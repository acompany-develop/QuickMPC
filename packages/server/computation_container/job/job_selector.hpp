#pragma once
#include <memory>

#include "external/proto/common_types/common_types.pb.h"
#include "job_parameter.hpp"
#include "jobs/correl_job.hpp"
#include "jobs/join_table_job.hpp"
#include "jobs/math_job.hpp"
#include "jobs/mesh_code_job.hpp"
#include "logging/logger.hpp"
#include "math/math.hpp"

namespace qmpc::Job
{
// Protocol Buffers の定義を引っ張ってくる
using ComputationMethod = pb_common_types::ComputationMethod;
class JobSelector
{
public:
    /*
        Jobを増やす際に下記関数に追加する
        method id: 関数
        4 :　相関係数Job
    */
    std::unique_ptr<Interface> operator()(const JobParameter &request)
    {
        ComputationMethod method_id = request.getRequest().method_id();
        QMPC_LOG_INFO("method id is: {}", method_id);
        switch (method_id)
        {
            case ComputationMethod::COMPUTATION_METHOD_MEAN:
                return std::make_unique<MathJob>(qmpc::Math::smean, request);
                break;
            case ComputationMethod::COMPUTATION_METHOD_VARIANCE:
                return std::make_unique<MathJob>(qmpc::Math::variance, request);
                break;
            case ComputationMethod::COMPUTATION_METHOD_SUM:
                return std::make_unique<MathJob>(qmpc::Math::sum, request);
                break;
            case ComputationMethod::COMPUTATION_METHOD_CORREL:
                return std::make_unique<CorrelJob>(request);
                break;
            case ComputationMethod::COMPUTATION_METHOD_MESH_CODE:
                return std::make_unique<MeshCodeJob>(request);
            case ComputationMethod::COMPUTATION_METHOD_JOIN_TABLE:
                return std::make_unique<JoinTableJob>(request);
                break;
            default:
                return nullptr;
        }
    }
};
}  // namespace qmpc::Job
