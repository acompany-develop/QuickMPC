#pragma once

#include "Client/ComputationToDbGate/Client.hpp"
#include "ConfigParse/ConfigParse.hpp"
#include "Math/Math.hpp"
#include "Model/ModelBase.hpp"
#include "Share/Compare.hpp"
#include "nlohmann/json.hpp"

namespace qmpc::Model
{
class DecisionTree : public ModelBase
{
    auto parse_table(const std::vector<std::vector<Share>> &table, const std::list<int> &arg) const
    {
        int sample_size = table.size();
        int dim = arg.size();

        /**
         * | x_11 ... x_1d |
         * |  .         .  |
         * | x_s1 ... x_sd |
         */
        std::vector<std::vector<Share>> x;
        x.reserve(sample_size);
        for (int i = 0; i < sample_size; ++i)
        {
            std::vector<Share> tmp;
            tmp.reserve(dim);
            for (const auto &j : arg)
            {
                tmp.emplace_back(table[i][j - 1]);
            }
            x.emplace_back(tmp);
        }
        return x;
    }

    auto classify(const nlohmann::json &dtree, const std::vector<Share> &x) const
    {
        if (dtree.contains("output"))
        {
            return Share(dtree["output"].get<std::string>());
        }
        auto bv = dtree["feature_bitvector"].get<std::vector<std::string>>();
        auto bv_s = std::vector<Share>();
        bv_s.reserve(bv.size());
        std::transform(
            bv.begin(),
            bv.end(),
            std::back_inserter(bv_s),
            [&](const std::string &b) { return FixedPoint(b); }
        );
        auto val = qmpc::Math::sum(x * bv_s);

        auto threshold = Share(dtree["threshold"].get<std::string>());
        if (val < threshold)
        {
            return classify(dtree["left_child"], x);
        }
        else
        {
            return classify(dtree["right_child"], x);
        }
    }

public:
    auto predict_f(const nlohmann::json &dtree_list, const std::vector<Share> &x) const
    {
        auto sum = Share();
        for (const nlohmann::json &dtree : dtree_list)
        {
            auto w = Share(dtree["weight"].get<std::string>());
            auto out = classify(dtree["tree_structure"], x);
            sum += w * out;
        }
        return sum;
    }

    std::vector<Share> predict(
        const std::vector<std::vector<Share>> &table,
        const std::vector<std::string> &schemas,
        const std::list<int> &arg,
        const std::string &model_param_job_uuid
    ) const override
    {
        auto db_client = qmpc::ComputationToDbGate::Client::getInstance();
        auto dtree_list = db_client->readModelparamJson(model_param_job_uuid);
        auto x = parse_table(table, arg);

        std::vector<Share> result;
        result.reserve(x.size());
        for (const auto &xi : x)
        {
            result.emplace_back(predict_f(dtree_list, xi));
        }
        return result;
    }
};
}  // namespace qmpc::Model