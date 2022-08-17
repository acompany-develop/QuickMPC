#pragma once
#include <iostream>
#include <memory>
#include <vector>

#include "OptInterface.hpp"
#include "Share/Share.hpp"
namespace qmpc::Optimizer
{

class SGD : public qmpc::Optimizer::OptInterface
{
    using Share = ::Share;
    using interface = qmpc::ObjectiveFunction::ObjectiveFunctionInterface;
    const Share alpha;
    const int batch_size;

public:
    SGD(const Share &s, int batch_size) : alpha(s), batch_size(batch_size) {}
    SGD(const FixedPoint &fp, int batch_size) : SGD(qmpc::Share::getConstantShare(fp), batch_size)
    {
    }

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
        auto dfx = f.df(batch_size, theta);
        open(dfx);
        auto test_dfx = recons(dfx);
        for (size_t i = 0; i < sz; ++i)
        {
            next_theta[i] -= alpha * dfx[i];
            // spdlog::info("df {} {}",i,test_dfx[i]);
        }
        return optimize(iterationNum - 1, f, next_theta);
    }
};
}  // namespace qmpc::Optimizer