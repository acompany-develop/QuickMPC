#pragma once

#include <iostream>
#include <list>

#include "Job/JobBase.hpp"
#include "Math/Math.hpp"
#include "Share/Matrix.hpp"
// TODO: 最適化関数と損失関数は全て同じインクルードにまとめる
#include "LogHeader/Logger.hpp"
#include "ObjectiveFunction/CrossEntropyError.hpp"
#include "Optimizer/optimizer.hpp"

namespace qmpc::Job
{

class LogisticRegressionJob : public JobBase<LogisticRegressionJob>
{
    using JobBase<LogisticRegressionJob>::JobBase;

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
         * | y1 ... yn |
         */
        std::vector<Share> y(sample_size);
        int j = arg[1].front();
        for (int i = 0; i < sample_size; ++i)
        {
            y[i] = table[i][j - 1];
        }

        return std::pair<std::vector<std::vector<Share>>, std::vector<Share>>{x, y};
    }

public:
    std::vector<Share> compute(
        const std::vector<std::vector<Share>> &table,
        const std::vector<std::string> &schemas,
        const std::vector<std::list<int>> &arg
    )
    {
        // 入力parse
        auto [x, y] = parse_table(table, arg);

        qmpc::ObjectiveFunction::CrossEntropyError function(x, y);
        // 探索のハイパーパラメータ，現在は0.1
        // 最急降下法でパラメータ最適化
        // auto opt =
        // qmpc::Optimizer::GradientDescent(qmpc::Share::getConstantShare(FixedPoint("1")));
        // 確率的勾配降下方
        int batch_size = std::size(x) / 2;
        auto opt = qmpc::Optimizer::SGD(qmpc::Share::getConstantShare(FixedPoint("1")), batch_size);
        //ロジスティック回帰の重み
        std::vector<Share> theta(x[0].size());
        // 繰り返し回数
        // TODO:一時的に固定
        constexpr int iterateNum = 100;
        auto ret = opt.optimize(iterateNum, function, theta);
#ifdef DEBUG
        open(y);
        auto y_rec = recons(y);
        for (auto a : y_rec)
        {
            spdlog::info("t is {}", a);
        }
        for (size_t i = 0; i < x.size(); ++i)
        {
            auto yy = qmpc::Math::open_sigmoid_vector(x[i] * ret);
            open(yy);
            auto yy_rec = recons(yy);
            spdlog::info("y is {}", yy_rec);
        }
#endif
        return ret;
    }
};
}  // namespace qmpc::Job