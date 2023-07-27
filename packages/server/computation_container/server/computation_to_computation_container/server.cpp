#include "server.hpp"

#include "config_parse/config_parse.hpp"
#include "logging/logger.hpp"
#include "share/address_id.hpp"

namespace qmpc::ComputationToComputation
{

Server::Server() noexcept
{
    Config *conf = Config::getInstance();
    for (int pt_id = 1; pt_id <= conf->n_parties; ++pt_id)
    {
        if (pt_id != conf->party_id)
        {
            auto client = Client::getPtr(conf->ip_addr_map[pt_id]);
            ccClients[pt_id] = std::move(client);
            QMPC_LOG_INFO("{:<15} Client {:<30} is Active", "[Cc2Cc]", pt_id);
        }
    }
}

// 複数シェアをexchangeする場合
grpc::Status Server::ExchangeShares(
    grpc::ServerContext *context,
    grpc::ServerReader<computationtocomputation::Shares> *stream,
    google::protobuf::Empty *response
)
{
    computationtocomputation::Shares multiple_shares;
    bool first = true;
    int party_id, share_id, job_id, thread_id;
    std::vector<CtoCShare> share_vec;

    while (stream->Read(&multiple_shares))
    {
        if (first)
        {
            auto address = multiple_shares.address_id();
            party_id = address.party_id();
            share_id = address.share_id();
            job_id = address.job_id();
            thread_id = address.thread_id();
        }
        // std::cout << "party_id is " << party_id << std::endl;
        // std::cout << "share_id is " << share_id << std::endl;
        // std::cout << "job_id is " << job_id << std::endl;
        // std::cout << "thread_id is " << thread_id << std::endl;
        for (int i = 0; i < multiple_shares.share_list_size(); i++)
        {
            auto share = multiple_shares.share_list(i);
            share_vec.emplace_back(share);
        }
        first = false;
    }

    std::lock_guard<std::mutex> lock(mtx);  // mutex発動
    if (!first)
    {
        shares_vec[std::make_tuple(party_id, share_id, job_id, thread_id)] = share_vec;
    }

    cond.notify_all();  // 通知
    return grpc::Status::OK;
}

void Server::runServer(std::string endpoint)
{
    auto server = Server::getServer();
    grpc::ServerBuilder builder;
    builder.RegisterService(server);

    runServerCore(builder, "Cc2Cc", endpoint);
}
}  // namespace qmpc::ComputationToComputation
