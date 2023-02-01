#pragma once

#include <grpc/grpc.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>

#include "external/proto/ManageToComputationContainer/manage_to_computation.grpc.pb.h"

namespace qmpc::ManageToComputation
{
class Server final : public managetocomputation::ManageToComputation::Service
{
public:
    Server() {}
    grpc::Status ExecuteComputation(
        grpc::ServerContext *context,
        const managetocomputation::ExecuteComputationRequest *request,
        google::protobuf::Empty *response
    ) override;
    grpc::Status CheckProgress(
        grpc::ServerContext *context,
        const managetocomputation::CheckProgressRequest *request,
        pb_common_types::JobProgress *response
    ) override;
};
void runServer(std::string endpoint);
}  // namespace qmpc::ManageToComputation
