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
#include <unordered_map>
#include <vector>

#include "Client/ComputationToComputationContainer/Client.hpp"
#include "Client/Helper/Helper.hpp"
#include "LogHeader/Logger.hpp"
#include "Logging/Logger.hpp"
#include "external/Proto/ComputationToComputationContainer/computation_to_computation.grpc.pb.h"
#include "unistd.h"
namespace qmpc::ComputationToComputation
{
class Server final : public computationtocomputation::ComputationToComputation::Service
{
    std::map<int, std::shared_ptr<Client>> ccClients;

    std::mutex mtx;
    std::condition_variable cond;
    Server() noexcept;
    ~Server() noexcept {}

public:
    grpc::Status ExchangeShare(
        grpc::ServerContext *context,
        const computationtocomputation::Share *share,
        google::protobuf::Empty *response
    ) override;
    grpc::Status ExchangeShares(
        grpc::ServerContext *context,
        grpc::ServerReader<computationtocomputation::Shares> *stream,
        google::protobuf::Empty *response
    ) override;
    // 受け取ったシェアをgetするメソッド
    std::string getShare(int party_id, qmpc::Share::AddressId share_id);
    std::vector<std::string> getShares(
        int party_id, const std::vector<qmpc::Share::AddressId> &share_ids, unsigned int length
    );
    Server(Server &&) noexcept = delete;
    Server(const Server &) noexcept = delete;
    Server &operator=(Server &&) noexcept = delete;
    Server &operator=(const Server &) noexcept = delete;
    void createClient();
    const std::shared_ptr<Client> getClient(const int &pt_id) const &
    {
        if (ccClients.count(pt_id) == 0)
        {
            spdlog::error("pt_id: {} Client is non existant.", pt_id);
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
    using address = std::tuple<int, int, unsigned int, int>;
    // 受け取ったシェアを保存する変数
    // party_id, share_idをキーとして保存
    std::map<address, std::string> shares;
};

}  // namespace qmpc::ComputationToComputation