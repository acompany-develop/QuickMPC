#pragma once

#include "Model/ModelBase.hpp"
#include "Model/Models/LinearRegression.hpp"

#include "ConfigParse/ConfigParse.hpp"
#include "Share/Networking.hpp"
#include "Server/ComputationToComputationContainer/Server.hpp"
#include "Client/ComputationToDbGate/Client.hpp"
#include "Math/Math.hpp"

namespace qmpc::Model
{
class LogisticRegression : public ModelBase
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
        auto lr = std::make_unique<LinearRegression>()->predict_f(a, x);
        return qmpc::Math::open_sigmoid(lr);
    }

    std::vector<Share> predict(
        const std::vector<std::vector<Share>> &table,
        const std::vector<std::string> &schemas,
        const std::list<int> &arg,
        const std::string &model_param_job_uuid
    ) const override
    {
        /*
         * 全てShareとして 1 / (1 + e^{-(a0 + a1x1 + ... + anxn)}) を計算する
         * 1. [A] = [w0] + [w1][x1] + ... + [wn][xn]として，[A]を復元する:
         * 線形回帰
         * 2. E = e^{-A} を平文で計算してShare化する
         * 3. [1] / ([1] + [E]) を計算する
         */

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