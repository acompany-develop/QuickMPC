#pragma once

#include "Client/ComputationToDbGate/Client.hpp"
#include "ConfigParse/ConfigParse.hpp"
#include "Model/ModelBase.hpp"

namespace qmpc::Model
{
class LinearRegression : public ModelBase
{
    auto parse_table(const std::vector<std::vector<Share>> &table, const std::list<int> &arg) const
    {
        int sample_size = table.size();
        int dim = arg.size();
        // 定数1のShare
        auto one = qmpc::Share::getConstantShare(FixedPoint("1"));

        /**
         * | 1 x_11 ... x_1d |
         * | .  .         .  |
         * | 1 x_s1 ... x_sd |
         */
        std::vector<std::vector<Share>> x;
        x.reserve(sample_size);
        for (int i = 0; i < sample_size; ++i)
        {
            std::vector<Share> tmp;
            tmp.reserve(dim + 1);
            tmp.emplace_back(one);
            for (const auto &j : arg)
            {
                tmp.emplace_back(table[i][j - 1]);
            }
            x.emplace_back(tmp);
        }
        return x;
    }

public:
    auto predict_f(const std::vector<Share> &a, const std::vector<Share> &x) const
    {
        auto sum = Share();
        for (const auto &ax : a *x)
        {
            sum += ax;
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
        auto a_str = db_client->readModelparam(model_param_job_uuid);
        auto a = std::vector<Share>(a_str.begin(), a_str.end());
        auto x = parse_table(table, arg);

        std::vector<Share> result;
        result.reserve(a.size());
        for (const auto &xi : x)
        {
            result.emplace_back(predict_f(a, xi));
        }
        return result;
    }
};
}  // namespace qmpc::Model