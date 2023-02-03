#pragma once

#include "external/proto/manage_to_computation_container/manage_to_computation.grpc.pb.h"
namespace qmpc::Job
{
class JobParameter
{
    managetocomputation::ExecuteComputationRequest request;
    unsigned int job_id;

public:
    JobParameter(const managetocomputation::ExecuteComputationRequest &req, unsigned int _job_id)
    {
        request = req;
        job_id = _job_id;
    }
    managetocomputation::ExecuteComputationRequest getRequest() const { return request; }
    unsigned int getJobId() const { return job_id; }
};
}  // namespace qmpc::Job
