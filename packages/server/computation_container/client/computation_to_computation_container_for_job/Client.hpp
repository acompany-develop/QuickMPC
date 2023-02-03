#pragma once

#include <grpc/grpc.h>
#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>

#include <string>

#include "Client/Helper/Helper.hpp"
#include "config_parse/config_parse.hpp"
#include "Share/AddressId.hpp"
#include "external/proto/ComputationToComputationContainerForJob/computation_to_computation_for_job.grpc.pb.h"

namespace qmpc::ComputationToComputationForJob
{
class Client
{
private:
    std::unique_ptr<computationtocomputationforjob::ComputationToComputationForJob::Stub> stub_;
    Client() noexcept = default;
    ~Client() noexcept = default;
    Client(const Url &endpoint) noexcept;
    Client &operator=(Client &&) noexcept = default;

public:
    static std::shared_ptr<Client> getPtr(const Url &);
    Client(Client &&) noexcept = delete;
    Client(const Client &) noexcept = delete;
    Client &operator=(const Client &) noexcept = delete;
    bool executeComputeFromSP(
        const managetocomputation::ExecuteComputationRequest &mc2cc_exec_comp_request,
        const unsigned int job_id,
        const bool is_job_trigger_party
    ) const;
};

computationtocomputationforjob::ExecuteComputeFromSPRequest convertExecuteComputationRequest(
    const managetocomputation::ExecuteComputationRequest &mc2cc_exec_comp_request
);

}  // namespace qmpc::ComputationToComputationForJob
