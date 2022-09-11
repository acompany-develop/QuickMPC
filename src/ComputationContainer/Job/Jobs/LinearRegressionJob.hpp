#pragma once

#include <list>

#include "ConfigParse/ConfigParse.hpp"
#include "Job/JobBase.hpp"
#include "Share/Matrix.hpp"

namespace qmpc::Job
{
class LinearRegressionJob : public JobBase<LinearRegressionJob>
{
    using JobBase<LinearRegressionJob>::JobBase;

    auto parse_table(
        const std::vector<std::vector<Share>> &table, const std::vector<std::list<int>> &arg
    ) const
    {
        int sample_size = table.size();
        int dim = arg[0].size();
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
            for (const auto &j : arg[0])
            {
                tmp.emplace_back(table[i][j - 1]);
            }
            x.emplace_back(tmp);
        }

        /**
         * | y1 |
         * | .  |
         * | yn |
         */
        std::vector<std::vector<Share>> y(sample_size, std::vector<Share>(1));
        int j = arg[1].front();
        for (int i = 0; i < sample_size; ++i)
        {
            y[i][0] = table[i][j - 1];
        }

        return std::pair<std::vector<std::vector<Share>>, std::vector<std::vector<Share>>>{x, y};
    }

public:
    std::vector<Share> compute(
        const std::vector<std::vector<Share>> &table,
        const std::vector<std::string> &schemas,
        const std::vector<std::list<int>> &arg
    )
    {
        spdlog::info("[progress] Linear Regression: learning: pre-processing (0/4)");
        // 入力parse
        auto [x, y] = parse_table(table, arg);
        if (x.empty())
        {
            return std::vector<Share>{};
        }

        // 行列に変換
        auto mat_x = qmpc::Share::ShareMatrix(x);
        auto mat_y = qmpc::Share::ShareMatrix(y);

        // w = (X^T * X)^{-1} * X^T * y
        spdlog::info("[progress] Linear Regression: learning: mat mul (1/4)");
        auto mat_x_trans = mat_x.transpose();
        auto mat_mul = (mat_x_trans * mat_x);

        spdlog::info("[progress] Linear Regression: learning: mat inverse (2/4)");
        auto mat_mul_inv = mat_mul.inverse();

        spdlog::info("[progress] Linear Regression: learning: mat mul (3/4)");
        auto w = mat_mul_inv * mat_x_trans * mat_y;

        spdlog::info("[progress] Linear Regression: learning: post-procesing (4/4)");

        return w.transpose().get_row()[0];
    }
};
}  // namespace qmpc::Job
