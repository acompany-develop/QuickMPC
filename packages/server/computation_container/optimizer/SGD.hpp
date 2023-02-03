#pragma once
#include <iostream>
#include <memory>
#include <vector>

#include "job/ProgressManager.hpp"
#include "OptInterface.hpp"
#include "share/share.hpp"

namespace qmpc::Optimizer
{

class SGD : public qmpc::Optimizer::OptInterface
{
    using Share = ::Share;
    using interface = qmpc::ObjectiveFunction::ObjectiveFunctionInterface;
    const Share alpha;
    const int batch_size;

    std::vector<Share> optimize_(
        int iterationNum,
        const interface &f,
        const std::vector<Share> &theta,
        Job::ScopedProgress<Job::ProgressIters_<Job::ProgressOrder::DESCENDING>> &progress
    ) const
    {
        progress->update(iterationNum);
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
            // QMPC_LOG_INFO("df {} {}",i,test_dfx[i]);
        }
        return optimize_(iterationNum - 1, f, next_theta, progress);
    }

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
        auto progress = Job::ProgressManager::getInstance()
                            ->createProgress<Job::ProgressIters_<Job::ProgressOrder::DESCENDING>>(
                                alpha.getId().getJobId(), "SGD optimize", iterationNum
                            );
        return optimize_(iterationNum, f, theta, progress);
    }
};
}  // namespace qmpc::Optimizer