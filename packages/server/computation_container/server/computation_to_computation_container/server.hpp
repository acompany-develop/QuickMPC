#pragma once

#include <grpc/grpc.h>
#include <grpcpp/security/server_credentials.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>

#include <condition_variable>
#include <iostream>
#include <map>
#include <mutex>
#include <sstream>  // 文字列分割を実装する際に用いる
#include <string>
#include <tuple>
#include <vector>

#include "client/computation_to_computation_container/client.hpp"
#include "client/helper/helper.hpp"
#include "external/proto/computation_to_computation_container/computation_to_computation.grpc.pb.h"
#include "logging/logger.hpp"
#include "server/helper/helper.hpp"
#include "unistd.h"
namespace qmpc::ComputationToComputation
{
using CtoCShare = computationtocomputation::Shares_Share;

class Server final : public computationtocomputation::ComputationToComputation::Service
{
    std::map<int, std::shared_ptr<Client>> ccClients;

    std::mutex mtx;
    std::condition_variable cond;
    Server() noexcept;
    ~Server() noexcept {}

public:
    grpc::Status ExchangeShares(
        grpc::ServerContext *context,
        grpc::ServerReader<computationtocomputation::Shares> *stream,
        google::protobuf::Empty *response
    ) override;
    // 受け取ったシェアをgetするメソッド
    template <typename SV>
    SV getShare(int party_id, qmpc::Share::AddressId share_id)
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
        auto share = shares_vec[key][0];
        shares_vec.erase(key);
        return toSV(share);
    }
    template <typename SV>
    std::vector<SV> getShares(int party_id, const std::vector<qmpc::Share::AddressId> &share_ids)
    {
        const std::size_t length = share_ids.size();
        if (length == 0)
        {
            return std::vector<CtoCShare>{};
        }

        Config *conf = Config::getInstance();
        // std::cout << "party share job thread"
        //           << " " << party_id << " " << share_ids[0].getShareId() << " "
        //           << share_ids[0].getJobId() << " " << share_ids[0].getThreadId() << std::endl;
        std::vector<CtoCShare> str_values;
        str_values.reserve(length);
        auto key = std::make_tuple(
            party_id, share_ids[0].getShareId(), share_ids[0].getJobId(), share_ids[0].getThreadId()
        );
        std::unique_lock<std::mutex> lock(mtx);  // mutex発動

        if (!cond.wait_for(
                lock,
                std::chrono::seconds(conf->getshare_time_limit * length),
                [&] { return shares_vec.count(key) == 1; }
            ))  // 待機
        {
            qmpc::Log::throw_with_trace(std::runtime_error("getShares is timeout"));
        }
        auto local_str_shares = shares_vec[key];
        shares_vec.erase(key);
        assert(local_str_shares.size() == length);
        std::vector<SV> ret(length);
        for (size_t i = 0; i < length; i++)
        {
            ret[i] = toSV(local_str_shares[i]);
        }
        return ret;
    }
    Server(Server &&) noexcept = delete;
    Server(const Server &) noexcept = delete;
    Server &operator=(Server &&) noexcept = delete;
    Server &operator=(const Server &) noexcept = delete;
    void createClient();
    const std::shared_ptr<Client> getClient(const int &pt_id) const &
    {
        if (ccClients.count(pt_id) == 0)
        {
            QMPC_LOG_ERROR("pt_id: {} Client is non existant.", pt_id);
            qmpc::Log::throw_with_trace(std::runtime_error("Client non-existant"));
        }
        return ccClients.at(pt_id);
    }
    static void runServer(std::string endpoint);

    static auto getServer()
    {
        static Server server;
        return &server;
    }

private:
    // party_id,share_id,job_id,thread_id
    using address_type = std::tuple<int, int, unsigned int, int>;
    // 受け取ったシェアを保存する変数
    // party_id, share_idをキーとして保存
    std::map<address_type, std::vector<CtoCShare>> shares_vec;

    template <typename SV>
    SV toSV(const CtoCShare &share_value)
    {
        if constexpr (std::is_same_v<SV, bool>)
        {
            assert(share_value.has_flag());
            return share_value.flag();
        }
        else if constexpr (std::is_integral_v<SV>)
        {
            assert(share_value.has_num());
            return share_value.num();
        }
        else
        {
            assert(share_value.has_byte());
            return SV(share_value.byte());
        }
    }
};

}  // namespace qmpc::ComputationToComputation
