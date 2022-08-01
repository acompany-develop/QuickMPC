#pragma once

#include <vector>
#include <memory>

#include "external/Proto/ManageToComputationContainer/manage_to_computation.grpc.pb.h"
#include "external/Proto/common_types/common_types.pb.h"

#include "Model/ModelBase.hpp"
#include "Model/Models/LinearRegression.hpp"
#include "Model/Models/LogisticRegression.hpp"
#include "Model/Models/DecisionTree.hpp"
#include "Model/Models/SID3Model.hpp"

#include "LogHeader/Logger.hpp"

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

public:
    /*
        model_id error 0
        readDB error 1
        predict error 2
        complete 3
    */
    auto push(const managetocomputation::PredictRequest &request) const
    {
        spdlog::info("Model Manager: Model Id is {}", request.model_id());
        auto model = select(request.model_id());
        if (!model)
        {
            spdlog::error("unknown Model Id");
            spdlog::error("Request Failed");
            return 0;
        }

        auto f = std::async(
            std::launch::async,
            [&request, model = std::move(model)]() { return model->run(request); }
        );
        return f.get();
    }
};
}  // namespace qmpc::Model
