#pragma once

#include "external/Proto/ManageToComputationContainer/manage_to_computation.grpc.pb.h"
#include <grpc/grpc.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>

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
    grpc::Status Predict(
        grpc::ServerContext *context,
        const managetocomputation::PredictRequest *request,
        google::protobuf::Empty *response
    ) override;
};
void runServer(std::string endpoint);
}  // namespace qmpc::ManageToComputation
