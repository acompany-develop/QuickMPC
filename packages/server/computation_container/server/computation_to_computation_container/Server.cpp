#include "Server.hpp"

#include "config_parse/config_parse.hpp"
#include "Logging/Logger.hpp"
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
            QMPC_LOG_INFO("{:<15} Client {:<30} is Active", "[Cc2Cc]", pt_id);
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
    // QMPC_LOG_INFO("log: server get share_id is {}, party_id is {}, value is {}",
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
    Config *conf = Config::getInstance();
    std::unique_lock<std::mutex> lock(mtx);  // mutex発動
    auto key = std::make_tuple(
        party_id, share_id.getShareId(), share_id.getJobId(), share_id.getThreadId()
    );
    if (!cond.wait_for(
            lock,
            std::chrono::seconds(conf->getshare_time_limit),
            [&] { return shares.count(key) == 1; }
        ))  // 待機
    {
        qmpc::Log::throw_with_trace(std::runtime_error("getShare is timeout"));
    }
    std::string share = shares[key];
    shares.erase(key);
    return share;
}

// 複数シェアget用
std::vector<std::string> Server::getShares(
    int party_id, const std::vector<qmpc::Share::AddressId> &share_ids, unsigned int length
)
{
    Config *conf = Config::getInstance();
    std::vector<std::string> str_values;
    str_values.reserve(length);
    for (unsigned int i = 0; i < length; i++)
    {
        std::unique_lock<std::mutex> lock(mtx);  // mutex発動
        auto key = std::make_tuple(
            party_id, share_ids[i].getShareId(), share_ids[i].getJobId(), share_ids[i].getThreadId()
        );
        if (!cond.wait_for(
                lock,
                std::chrono::seconds(conf->getshare_time_limit),
                [&] { return shares.count(key) == 1; }
            ))  // 待機
        {
            qmpc::Log::throw_with_trace(std::runtime_error("getShares is timeout"));
        }
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
    builder.RegisterService(server);

    runServerCore(builder, "Cc2Cc", endpoint);
}
}  // namespace qmpc::ComputationToComputation
