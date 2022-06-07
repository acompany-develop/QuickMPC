#include "Server.hpp"

#include "ConfigParse/ConfigParse.hpp"
#include "LogHeader/Logger.hpp"
#include "Share/AddressId.hpp"

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
            spdlog::info("{:<15} Client {:<30} is Active", "[Cc2Cc]", pt_id);
        }
    }
}
// 他のccから
// 単一シェアをexchangeする場合
grpc::Status Server::ExchangeShare(
    grpc::ServerContext *context,
    const computationtocomputation::Share *share,
    google::protobuf::Empty *response
)
{
    std::lock_guard<std::mutex> lock(mtx);  // mutex発動
    // spdlog::info("log: server get share_id is {}, party_id is {}, value is {}",
    //              share->share_id(),
    //              share->party_id(),
    //              share->value());
    auto address = share->address_id();
    switch (share->value_case())
    {
        using cs = computationtocomputation::Share;
        case (cs::ValueCase::kFlag):
            shares[std::make_tuple(
                share->party_id(), address.share_id(), address.job_id(), address.thread_id()
            )] = std::to_string(share->flag());
            break;
        case (cs::ValueCase::kNum):
            shares[std::make_tuple(
                share->party_id(), address.share_id(), address.job_id(), address.thread_id()
            )] = std::to_string(share->num());
            break;
        case (cs::ValueCase::kNum64):
            shares[std::make_tuple(
                share->party_id(), address.share_id(), address.job_id(), address.thread_id()
            )] = std::to_string(share->num64());
            break;
        case (cs::ValueCase::kF):
            shares[std::make_tuple(
                share->party_id(), address.share_id(), address.job_id(), address.thread_id()
            )] = std::to_string(share->f());
            break;
        case (cs::ValueCase::kD):
            shares[std::make_tuple(
                share->party_id(), address.share_id(), address.job_id(), address.thread_id()
            )] = std::to_string(share->d());
            break;
        case (cs::ValueCase::kByte):
        case (cs::ValueCase::VALUE_NOT_SET):
            shares[std::make_tuple(
                share->party_id(), address.share_id(), address.job_id(), address.thread_id()
            )] = share->byte();
            break;
    }
    cond.notify_all();  // 通知
    return grpc::Status::OK;
}

// 複数シェアをexchangeする場合
grpc::Status Server::ExchangeShares(
    grpc::ServerContext *context,
    grpc::ServerReader<computationtocomputation::Shares> *stream,
    google::protobuf::Empty *response
)
{
    std::lock_guard<std::mutex> lock(mtx);  // mutex発動

    using cs = computationtocomputation::Shares_Share;
    computationtocomputation::Shares multiple_shares;
    while (stream->Read(&multiple_shares))
    {
        int party_id = multiple_shares.party_id();
        for (int i = 0; i < multiple_shares.share_list_size(); i++)
        {
            auto share = multiple_shares.share_list(i);
            auto address = share.address_id();
            switch (share.value_case())
            {
                case (cs::ValueCase::kFlag):
                    shares[std::make_tuple(
                        party_id, address.share_id(), address.job_id(), address.thread_id()
                    )] = std::to_string(share.flag());
                    break;
                case (cs::ValueCase::kNum):
                    shares[std::make_tuple(
                        party_id, address.share_id(), address.job_id(), address.thread_id()
                    )] = std::to_string(share.num());
                    break;
                case (cs::ValueCase::kNum64):
                    shares[std::make_tuple(
                        party_id, address.share_id(), address.job_id(), address.thread_id()
                    )] = std::to_string(share.num64());
                    break;
                case (cs::ValueCase::kF):
                    shares[std::make_tuple(
                        party_id, address.share_id(), address.job_id(), address.thread_id()
                    )] = std::to_string(share.f());
                    break;
                case (cs::ValueCase::kD):
                    shares[std::make_tuple(
                        party_id, address.share_id(), address.job_id(), address.thread_id()
                    )] = std::to_string(share.d());
                    break;
                case (cs::ValueCase::kByte):
                case (cs::ValueCase::VALUE_NOT_SET):
                    shares[std::make_tuple(
                        party_id, address.share_id(), address.job_id(), address.thread_id()
                    )] = share.byte();
                    break;
            }
        }
    }

    cond.notify_all();  // 通知
    return grpc::Status::OK;
}

// 単一シェアget用
std::string Server::getShare(int party_id, qmpc::Share::AddressId share_id)
{
    std::unique_lock<std::mutex> lock(mtx);  // mutex発動
    auto key = std::make_tuple(
        party_id, share_id.getShareId(), share_id.getJobId(), share_id.getThreadId()
    );
    cond.wait(lock, [&] { return shares.count(key) == 1; });  // 待機
    std::string share = shares[key];
    shares.erase(key);
    return share;
}

// 複数シェアget用
std::vector<std::string> Server::getShares(
    int party_id, const std::vector<qmpc::Share::AddressId> &share_ids, unsigned int length
)
{
    std::vector<std::string> str_values;
    str_values.reserve(length);
    for (unsigned int i = 0; i < length; i++)
    {
        std::unique_lock<std::mutex> lock(mtx);  // mutex発動
        auto key = std::make_tuple(
            party_id, share_ids[i].getShareId(), share_ids[i].getJobId(), share_ids[i].getThreadId()
        );
        cond.wait(lock, [&] { return shares.count(key) == 1; });  // 待機
        std::string share = shares[key];
        str_values.emplace_back(share);
        shares.erase(key);
    }
    return str_values;
}

void Server::runServer(std::string endpoint)
{
    auto server = Server::getServer();
    grpc::ServerBuilder builder;
    /*
    grpc Server keepalive設定
    GRPC_ARG_HTTP2_MIN_RECV_PING_INTERVAL_WITHOUT_DATA_MS　pingの間隔がこの時間より短い場合は攻撃とみなす　9秒
    （この時間はクライアントと調整する）
    GRPC_ARG_KEEPALIVE_PERMIT_WITHOUT_CALLS　通信がなくてもpingを送る　on
    */
    builder.AddChannelArgument(GRPC_ARG_HTTP2_MIN_RECV_PING_INTERVAL_WITHOUT_DATA_MS, 9000);
    builder.AddChannelArgument(GRPC_ARG_KEEPALIVE_PERMIT_WITHOUT_CALLS, 1);

    // 外部からのSSL通信はインフラレイヤーでhttpに変換するのでInsecureにする
    builder.AddListeningPort(endpoint, grpc::InsecureServerCredentials());
    builder.RegisterService(server);
    std::unique_ptr<grpc::Server> listener(builder.BuildAndStart());

    // HealthCheckService を有効化する
    grpc::HealthCheckServiceInterface *service = listener->GetHealthCheckService();
    if (service != nullptr)
    {
        service->SetServingStatus(true);  // 登録した全てのサービスで有効化
    }
    else
    {
        spdlog::info(
            "{:<15} grpc::DefaultHealthCheckService is not enabled on {:<30}", "[Cc2Cc]", endpoint
        );
    }

    spdlog::info("{:<15} Server listening on {:<30}", "[Cc2Cc]", endpoint);
    listener->Wait();
}
}  // namespace qmpc::ComputationToComputation
