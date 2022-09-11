#pragma once

#include <boost/range/adaptor/indexed.hpp>
#include <chrono>

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
        spdlog::info("[progress] Linear Regression: predict: pre-processing (0/2)");

        auto db_client = qmpc::ComputationToDbGate::Client::getInstance();
        auto a_str = db_client->readModelparam(model_param_job_uuid);
        auto a = std::vector<Share>(a_str.begin(), a_str.end());
        auto x = parse_table(table, arg);

        std::vector<Share> result;
        result.reserve(a.size());

        spdlog::info("[progress] Linear Regression: predict: compute (1/2)");
        auto time_from = std::chrono::system_clock::now();
        for (const auto &xi : boost::adaptors::index(x))
        {
            if (xi.index() % 1000 == 0)
            {
                auto time_to = std::chrono::system_clock::now();
                auto dur =
                    std::chrono::duration_cast<std::chrono::milliseconds>(time_to - time_from);

                if (dur.count() >= 5000)
                {
                    spdlog::info(
                        "[progress] Linear Regression: predict: compute (1/2): {:>5.2f} %",
                        xi.index() * 100.0 / x.size()
                    );
                    time_from = time_to;
                }
            }

            result.emplace_back(predict_f(a, xi.value()));
        }
        spdlog::info("[progress] Linear Regression: predict: compute (1/2): {:>5.2f} %", 100.0);

        spdlog::info("[progress] Linear Regression: predict: post-processing (2/2)");

        return result;
    }
};
}  // namespace qmpc::Model