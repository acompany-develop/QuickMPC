#pragma once
#include <iostream>
#include <memory>
#include <vector>

#include "OptInterface.hpp"
#include "Share/Share.hpp"
namespace qmpc::Optimizer
{
/*
最急降下法
    w_i+1 = w_i - alpha * df
*/
class GradientDescent : public qmpc::Optimizer::OptInterface
{
    using Share = ::Share;
    using interface = qmpc::ObjectiveFunction::ObjectiveFunctionInterface;
    const Share alpha;

public:
    GradientDescent(const ::Share &s) : alpha(s) {}
    GradientDescent(const FixedPoint &fp) : GradientDescent(qmpc::Share::getConstantShare(fp)) {}

    /*
    最適化を適応させる
    入力:試行回数、目的関数、重み
    */
    std::vector<Share> optimize(
        int iterationNum, const interface &f, const std::vector<Share> &theta
    ) const override
    {
        if (iterationNum == 0)
        {
            return theta;
        }
        size_t sz = std::size(theta);
        auto next_theta = theta;
        auto dfx = f.df(theta);
        open(dfx);
        auto test_dfx = recons(dfx);
        for (size_t i = 0; i < sz; ++i)
        {
            next_theta[i] -= alpha * dfx[i];
            // QMPC_LOG_INFO("df {} {}",i,test_dfx[i]);
        }
        return optimize(iterationNum - 1, f, next_theta);
    }
};
}  // namespace qmpc::Optimizer
