#pragma once
#include <vector>
#include <iostream>
#include "Share/Share.hpp"
#include "OptInterface.hpp"

namespace qmpc::Optimizer
{

/**
 * @brief 勾配の移動平均をとった最適化
 * 入力は勾配平均に対する重みβ、勾配に対するα、sgdへのバッチ数
 * v_k+1 = αv_k - βV
 * x_k+1 = x_k + v_k+1
 *
 */
class Momentum : public qmpc::Optimizer::OptInterface
{
    using Share = ::Share;
    using interface = qmpc::ObjectiveFunction::ObjectiveFunctionInterface;
    const ::FixedPoint beta;
    const ::FixedPoint alpha;
    const int batch_size;
    std::vector<Share> moment(
        int iterationNum,
        const std::vector<Share> &vt_,
        const std::vector<Share> &theta_,
        const interface &f
    ) const
    {
        size_t size = std::size(theta_);
        std::vector<Share> ret;
        auto vt = vt_;
        auto theta = theta_;
        for (int t = 0; t < iterationNum; ++t)
        {
            auto dfx = f.df(batch_size, theta);
            for (size_t i = 0; i < size; ++i)
            {
                vt[i] = beta * vt[i] + dfx[i];
                // spdlog::info("df {} {}",i,test_dfx[i]);
            }
            for (size_t i = 0; i < size; ++i)
            {
                theta[i] = theta[i] - alpha * vt[i];
                // spdlog::info("df {} {}",i,test_dfx[i]);
            }
        }
        return theta;
    }

public:
    Momentum(const ::FixedPoint &alpha, const ::FixedPoint &beta, int batch_size)
        : alpha(alpha), beta(beta), batch_size(batch_size)
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
        std::vector<Share> velocity(sz);
        auto ret = moment(iterationNum, velocity, theta, f);
        return ret;
    }
};
}  // namespace qmpc::Optimizer