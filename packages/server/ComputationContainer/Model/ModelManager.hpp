#pragma once

#include <memory>
#include <vector>

#include "Job/JobStatus.hpp"
#include "Job/ProgressManager.hpp"
#include "Logging/Logger.hpp"
#include "Model/ModelBase.hpp"
#include "Model/Models/DecisionTree.hpp"
#include "Model/Models/LinearRegression.hpp"
#include "Model/Models/LogisticRegression.hpp"
#include "Model/Models/SID3Model.hpp"
#include "external/proto/ManageToComputationContainer/manage_to_computation.grpc.pb.h"
#include "external/proto/common_types/common_types.pb.h"

namespace qmpc::Model
{

using PredictMethod = pb_common_types::PredictMethod;
class ModelManager
{
    std::unique_ptr<ModelBase> select(PredictMethod model_id) const
    {
        switch (model_id)
        {
            case PredictMethod::PREDICT_METHOD_LINEAR_REGRESSION:
                return std::make_unique<LinearRegression>();
            case PredictMethod::PREDICT_METHOD_LOGISTIC_REGRESSION:
                return std::make_unique<LogisticRegression>();
            case PredictMethod::PREDICT_METHOD_DECISION_TREE:
                return std::make_unique<DecisionTree>();
            case PredictMethod::PREDICT_METHOD_SID3_TREE:
                return std::make_unique<SID3Model>();
            default:
                return nullptr;
        }
    }

    // try-catchをしながら特定の処理を実行する
    template <class F>
    static auto try_catch_run(const std::string &job_uuid, const F &func)
    {
        qmpc::Job::StatusManager statusManager(job_uuid);
        try
        {
            return func();
        }
        catch (const std::runtime_error &e)
        {
            QMPC_LOG_ERROR("{}", static_cast<int>(statusManager.getStatus()));
            QMPC_LOG_ERROR("{} | Predict Error", e.what());

            auto error_info = boost::get_error_info<qmpc::Log::traced>(e);
            if (error_info)
            {
                QMPC_LOG_ERROR("{}", *error_info);
                statusManager.error(e, *error_info);
            }
            else
            {
                QMPC_LOG_ERROR("thrown exception has no stack trace information");
                statusManager.error(e, std::nullopt);
            }
        }
        catch (const std::exception &e)
        {
            QMPC_LOG_ERROR("unexpected Error");
            QMPC_LOG_ERROR("{} | Predict Error", e.what());

            auto error_info = boost::get_error_info<qmpc::Log::traced>(e);
            if (error_info)
            {
                QMPC_LOG_ERROR("{}", *error_info);
                statusManager.error(e, *error_info);
            }
            else
            {
                QMPC_LOG_ERROR("thrown exception has no stack trace information");
                statusManager.error(e, std::nullopt);
            }
        }
        return static_cast<int>(statusManager.getStatus());
    }

public:
    /*
        model_id error 0
        readDB error 1
        predict error 2
        complete 3
    */
    auto push(const managetocomputation::PredictRequest &request) const
    {
        QMPC_LOG_INFO("Model Manager: Model Id is {}", request.model_id());
        auto model = select(request.model_id());
        if (!model)
        {
            QMPC_LOG_ERROR("unknown Model Id");
            QMPC_LOG_ERROR("Request Failed");
            return 0;
        }

        auto f = std::async(
            std::launch::async,
            [&request, model = std::move(model)]()
            { return try_catch_run(request.job_uuid(), [&]() { return model->run(request); }); }
        );
        return f.get();
    }
};
}  // namespace qmpc::Model
