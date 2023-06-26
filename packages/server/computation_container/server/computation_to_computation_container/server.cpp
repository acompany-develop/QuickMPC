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

static std::string share_to_str(const computationtocomputation::Shares_Share &share)
{
    using cs = computationtocomputation::Shares_Share;
    switch (share.value_case())
    {
        case (cs::ValueCase::kFlag):
            return std::to_string(share.flag());
        case (cs::ValueCase::kNum):
            return std::to_string(share.num());
        case (cs::ValueCase::kNum64):
            return std::to_string(share.num64());
        case (cs::ValueCase::kF):
            return std::to_string(share.f());
        case (cs::ValueCase::kD):
            return std::to_string(share.d());
        case (cs::ValueCase::kByte):
        case (cs::ValueCase::VALUE_NOT_SET):
            return share.byte();
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
    std::vector<std::string> share_str_vec;

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

            auto share_str = share_to_str(share);
            share_str_vec.emplace_back(share_str);
        }
        first = false;
    }

    std::lock_guard<std::mutex> lock(mtx);  // mutex発動
    shares_vec[std::make_tuple(party_id, share_id, job_id, thread_id)] = share_str_vec;

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
            [&] { return shares_vec.count(key) == 1; }
        ))  // 待機
    {
        qmpc::Log::throw_with_trace(std::runtime_error("getShare is timeout"));
    }
    auto share = shares_vec[key];
    shares_vec.erase(key);
    return share[0];
}

// 複数シェアget用
std::vector<std::string> Server::getShares(
    int party_id, const std::vector<qmpc::Share::AddressId> &share_ids
)
{
    const size_t length = share_ids.size();
    if (length == 0)
    {
        return std::vector<std::string>{};
    }

    Config *conf = Config::getInstance();
    // std::cout << "party share job thread"
    //           << " " << party_id << " " << share_ids[0].getShareId() << " "
    //           << share_ids[0].getJobId() << " " << share_ids[0].getThreadId() << std::endl;
    std::vector<std::string> str_values;
    str_values.reserve(length);
    auto key = std::make_tuple(
        party_id, share_ids[0].getShareId(), share_ids[0].getJobId(), share_ids[0].getThreadId()
    );
    std::unique_lock<std::mutex> lock(mtx);  // mutex発動

    if (!cond.wait_for(
            lock,
            std::chrono::seconds(conf->getshare_time_limit),
            [&] { return shares_vec.count(key) == 1; }
        ))  // 待機
    {
        qmpc::Log::throw_with_trace(std::runtime_error("getShares is timeout"));
    }
    auto local_str_shares = shares_vec[key];
    shares_vec.erase(key);
    assert(local_str_shares.size() == length);
    return local_str_shares;
}

void Server::runServer(std::string endpoint)
{
    auto server = Server::getServer();
    grpc::ServerBuilder builder;
    builder.RegisterService(server);

    runServerCore(builder, "Cc2Cc", endpoint);
}
}  // namespace qmpc::ComputationToComputation
