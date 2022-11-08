#pragma once
#include <memory>

#include "JobParameter.hpp"
#include "Jobs/CorrelJob.hpp"
#include "Jobs/JoinTableJob.hpp"
#include "Jobs/LinearRegressionJob.hpp"
#include "Jobs/LogisticRegressionJob.hpp"
#include "Jobs/MathJob.hpp"
#include "Jobs/MeshCodeJob.hpp"
#include "Jobs/SID3Job.hpp"
#include "LogHeader/Logger.hpp"
#include "Math/Math.hpp"
#include "external/Proto/common_types/common_types.pb.h"

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
            case ComputationMethod::COMPUTATION_METHOD_LINEAR_REGRESSION:
                return std::make_unique<LinearRegressionJob>(request);
                break;
            case ComputationMethod::COMPUTATION_METHOD_LOGISTIC_REGRESSION:
                return std::make_unique<LogisticRegressionJob>(request);
                break;
            case ComputationMethod::COMPUTATION_METHOD_MESH_CODE:
                return std::make_unique<MeshCodeJob>(request);
                break;
            case ComputationMethod::COMPUTATION_METHOD_DECISION_TREE:
                // 現状は決定木をSID3で構築しているので，SID3という名前になっている（と思われる）
                return std::make_unique<SID3Job>(request);
                break;
            case ComputationMethod::COMPUTATION_METHOD_JOIN_TABLE:
                return std::make_unique<JoinTableJob>(request);
                break;
            default:
                return nullptr;
        }
    }
};
}  // namespace qmpc::Job
