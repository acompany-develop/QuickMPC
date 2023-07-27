#include <grpc/grpc.h>
#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>
#include <unistd.h>

#include <string>
#include <thread>
#include <vector>

#include "config_parse/config_parse.hpp"
#include "external/proto/computation_to_computation_container/computation_to_computation.grpc.pb.h"
#include "fixed_point/fixed_point.hpp"
#include "gtest/gtest.h"
#include "logging/logger.hpp"
#include "server/computation_to_computation_container/server.hpp"
#include "share/address_id.hpp"

void runServer(std::string endpoint)
{
    auto server = qmpc::ComputationToComputation::Server::getServer();
    grpc::ServerBuilder builder;
    /*
    grpc Server keepalive設定
    GRPC_ARG_HTTP2_MIN_RECV_PING_INTERVAL_WITHOUT_DATA_MS　pingの間隔がこの時間より短い場合は攻撃とみなす　9秒
    （この時間はクライアントと調整する）
    GRPC_ARG_KEEPALIVE_PERMIT_WITHOUT_CALLS　通信がなくてもpingを送る　on
    */
    builder.AddChannelArgument(GRPC_ARG_HTTP2_MIN_RECV_PING_INTERVAL_WITHOUT_DATA_MS, 9000);
    builder.AddChannelArgument(GRPC_ARG_KEEPALIVE_PERMIT_WITHOUT_CALLS, 1);
    builder.AddListeningPort(endpoint, grpc::InsecureServerCredentials());
    builder.RegisterService(server);
    std::unique_ptr<grpc::Server> listener(builder.BuildAndStart());
    QMPC_LOG_INFO("[CcToCc_test] Server listening on {}", endpoint);
    listener->Wait();
}

TEST(CtoC_Test, EXCHANGESHARE)
{
    Config *conf = Config::getInstance();
    std::vector<qmpc::Share::AddressId> share_id(1);
    computationtocomputation::Shares share;
    std::cout << "exchangeshare id " << share_id[0] << std::endl;
    int value = 10;
    auto a = share.mutable_address_id();
    a->set_share_id(share_id[0].getShareId());
    a->set_job_id(share_id[0].getJobId());
    a->set_party_id(conf->party_id);
    computationtocomputation::Shares_Share *multiple_shares = share.add_share_list();
    multiple_shares->set_byte(value);

    auto stub_ = createStub<computationtocomputation::ComputationToComputation>(
        Url::Parse("http://localhost:50120")
    );
    grpc::ClientContext context;
    google::protobuf::Empty response;
    std::shared_ptr<grpc::ClientWriter<computationtocomputation::Shares>> stream(
        stub_->ExchangeShares(&context, &response)
    );
    stream->Write(share);
    stream->WritesDone();
    grpc::Status status = stream->Finish();
    EXPECT_TRUE(status.ok());

    auto server = qmpc::ComputationToComputation::Server::getServer();
    int data = server->getShare<int>(conf->party_id, share_id[0]);
    EXPECT_EQ(value, data);
}
TEST(CtoC_Test, EXCHANGESHARES)
{
    Config *conf = Config::getInstance();
    unsigned int length = 2;
    std::vector<qmpc::Share::AddressId> share_ids(length);
    std::vector<int> values = {10, 11};
    computationtocomputation::Shares shares;
    auto a = shares.mutable_address_id();
    a->set_share_id(share_ids[0].getShareId());
    a->set_job_id(share_ids[0].getJobId());
    a->set_party_id(conf->party_id);
    for (unsigned int i = 0; i < length; i++)
    {
        computationtocomputation::Shares_Share *multiple_shares = shares.add_share_list();
        multiple_shares->set_byte(values[i]);
    }
    auto stub_ = createStub<computationtocomputation::ComputationToComputation>(
        Url::Parse("http://localhost:50120")
    );
    grpc::ClientContext context;
    google::protobuf::Empty response;
    std::shared_ptr<grpc::ClientWriter<computationtocomputation::Shares>> stream(
        stub_->ExchangeShares(&context, &response)
    );
    stream->Write(shares);
    stream->WritesDone();
    grpc::Status status = stream->Finish();
    EXPECT_TRUE(status.ok());

    auto server = qmpc::ComputationToComputation::Server::getServer();
    std::vector<int> datas = server->getShares<int>(conf->party_id, share_ids);
    for (unsigned int i = 0; i < length; i++)
    {
        EXPECT_EQ(values[i], datas[i]);
    }
}

TEST(CtoC_Test, GetShareThrowExceptionTest)
{
    Config *conf = Config::getInstance();
    qmpc::Share::AddressId share_id;

    auto server = qmpc::ComputationToComputation::Server::getServer();
    EXPECT_ANY_THROW(server->getShare<int>(conf->party_id, share_id));
}

TEST(CtoC_Test, GetSharesThrowExceptionTest)
{
    Config *conf = Config::getInstance();
    unsigned int length = 1;
    std::vector<qmpc::Share::AddressId> share_ids(length);

    auto server = qmpc::ComputationToComputation::Server::getServer();
    EXPECT_ANY_THROW(server->getShares<int>(conf->party_id, share_ids));
}
int main(int argc, char **argv)
{
    Config *conf = Config::getInstance();

    Url ctoc_ip = conf->ip_addr_map[conf->party_id];
    const std::string ctc_my_ip_str("0.0.0.0:" + ctoc_ip.port);
    std::thread th1(runServer, ctc_my_ip_str);
    sleep(3);

    ::testing::InitGoogleTest(&argc, argv);
    int ok = RUN_ALL_TESTS();
    if (ok == 0)
        std::exit(EXIT_SUCCESS);
    else
        std::exit(EXIT_FAILURE);

    th1.detach();
}
