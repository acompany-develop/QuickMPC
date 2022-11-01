#pragma once
#include <iostream>
#include <vector>

#include "OptInterface.hpp"
#include "Share/Share.hpp"

namespace qmpc::Optimizer
{

/**
 * @brief momentumとRSMpropを合わせた物
 * 入力は勾配平均に対する重みβ1、二乗和に対する重みβ2、学習に対する重みα、sgdへのバッチ数
 * v_k+1 = αv_k - βV
 * x_k+1 = x_k + v_k+1
 *
 */
class Adam : public qmpc::Optimizer::OptInterface
{
    using Share = ::Share;
    using interface = qmpc::ObjectiveFunction::ObjectiveFunctionInterface;
    using Shares = std::vector<Share>;
    const ::FixedPoint alpha;
    const ::FixedPoint beta1;
    const ::FixedPoint beta2;
    const ::FixedPoint eps;
    auto adapt(int iterationNum, const Shares &theta_, const interface &f) const
    {
        size_t size = std::size(theta_);
        // 勾配平均ベクトル
        Shares mt(size);
        // 勾配二乗平方根ベクトル
        Shares vt(size);
        Shares theta = theta_;
        double beta1_pow = 1.0;
        double beta2_pow = 1.0;

        for (int t = 0; t < iterationNum; ++t)
        {
            auto l1 = f.df(1, theta);
            auto l2 = l1 * l1;
            for (size_t i = 0; i < size; ++i)
            {
                mt[i] = beta1 * mt[i] + (1 - beta1) * l1[i];
                vt[i] = beta2 * vt[i] + (1 - beta2) * l2[i];
            }
            beta1_pow *= beta1.getDoubleVal();
            beta2_pow *= beta2.getDoubleVal();
            auto b_t = std::sqrt(1 - beta2_pow) / (1 - beta1_pow);
            for (size_t i = 0; i < size; ++i)
            {
                theta[i] = theta[i] - alpha * mt[i] * FixedPoint(b_t) / (sqrt(vt[i]) + eps);
            }
        }
        return theta;
    }

public:
    /**
     * @brief Construct a new Adam object
     *
     * @param alpha 学習係数
     * @param beta1 勾配平均係数1
     * @param beta2 勾配平均係数2
     * @param eps 勾配除算に関係する最小微少量
     */
    Adam(
        const ::FixedPoint &alpha = 0.1,
        const ::FixedPoint &beta1 = 0.9,
        const ::FixedPoint &beta2 = 0.99,
        const ::FixedPoint &eps = 0.00000001
    )
        : alpha(alpha), beta1(beta1), beta2(beta2), eps(eps)
    {
    }
    /*
    最適化を適応させる
    入力:試行回数、目的関数、重み
    */
    Shares optimize(int iterationNum, const interface &f, const Shares &theta) const override
    {
        return adapt(iterationNum, theta, f);
    }
};
}  // namespace qmpc::Optimizer
