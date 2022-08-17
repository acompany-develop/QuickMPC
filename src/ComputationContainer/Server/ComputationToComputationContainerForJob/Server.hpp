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

#include "Client/Helper/Helper.hpp"
#include "external/Proto/ComputationToComputationContainerForJob/computation_to_computation_for_job.grpc.pb.h"
#include "unistd.h"

namespace qmpc::ComputationToComputationForJob
{
class Server final : public computationtocomputationforjob::ComputationToComputationForJob::Service
{
    Server() noexcept;
    ~Server() noexcept {}

public:
    grpc::Status ExecuteComputeFromSP(
        grpc::ServerContext *context,
        const computationtocomputationforjob::ExecuteComputeFromSPRequest *request,
        google::protobuf::Empty *response
    ) override;
    Server(Server &&) noexcept = delete;
    Server(const Server &) noexcept = delete;
    Server &operator=(Server &&) noexcept = delete;
    Server &operator=(const Server &) noexcept = delete;
    static void runServer(std::string endpoint);

    static auto getServer()
    {
        static Server server;
        return &server;
    }
};

}  // namespace qmpc::ComputationToComputationForJob