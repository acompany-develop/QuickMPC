#pragma once
#include <random>

#include "Logging/Logger.hpp"
#include "ObjectiveFunctionInterface.hpp"
#include "Share/Share.hpp"
namespace qmpc::ObjectiveFunction
{
/*
交差エントロピー誤差関数
    L(W,T) = -T_n * log(sigmoid(W_n)) + (1 - T_n) * log(1-sigmoid(W_n))
    W(X) = X_0 * theta_0 + X_1 * theta_1 ...
*/
class CrossEntropyError : public ObjectiveFunctionInterface
{
    using Share = ::Share;
    std::vector<Share> t;
    std::vector<std::vector<Share>> x;

public:
    CrossEntropyError(const std::vector<std::vector<Share>> &x, const std::vector<Share> &t)
        : t(t), x(x)
    {
    }

    /*
    交差エントロピー誤差関数の導関数
    入力：ロジスティック回帰の重み配列theta
    出力：
    */
    std::vector<Share> df(const std::vector<Share> &theta) const override
    {
        // 偏微分値を計算
        size_t size = std::size(x);
        std::vector<Share> hy;
        hy.reserve(size);
        for (size_t i = 0; i < size; ++i)
        {
            auto y = qmpc::Math::open_sigmoid_vector(x[i] * theta);
            hy.emplace_back(y - t[i]);
        }
        int dim = theta.size();
        auto x_t = qmpc::Share::ShareMatrix(x).transpose().get_row();
        std::vector<Share> ret;
        ret.reserve(dim);
        for (int i = 0; i < dim; ++i)
        {
            ret.emplace_back(qmpc::Math::sum(hy * x_t[i]));
        }
        return ret;
    }
    /*
    ミニバッチ処理sgd導関数
     入力：個数num,重みtheta
     出力：勾配
    */
    std::vector<Share> df(int num, const std::vector<Share> &theta) const override
    {
        // 偏微分値を計算
        size_t x_size = std::size(x);
        auto shuffle_index = get_random_x(num, x_size);
        std::vector<std::vector<Share>> part_x;
        part_x.reserve(num);
        size_t part_x_size = std::size(shuffle_index);
        std::vector<Share> hy;
        hy.reserve(x_size);
        for (size_t i = 0; i < part_x_size; ++i)
        {
            part_x.emplace_back(x[shuffle_index[i]]);
            auto y = qmpc::Math::open_sigmoid_vector(part_x[i] * theta);
            hy.emplace_back(y - t[shuffle_index[i]]);
        }
        int dim = theta.size();
        auto part_x_t = qmpc::Share::ShareMatrix(part_x).transpose().get_row();
        std::vector<Share> ret;
        ret.reserve(dim);
        for (int i = 0; i < dim; ++i)
        {
            ret.emplace_back(qmpc::Math::sum(hy * part_x_t[i]));
        }
        return ret;
    }

    //適当に幾つかのインデックスを返す
    std::vector<int> get_random_x(int num, int size) const
    {
        if (num > size)
        {
            qmpc::Log::throw_with_trace(std::runtime_error("sgd index count error"));
        }

        Config *conf = Config::getInstance();
        std::vector<Share> s_index(num);
        std::vector<int> shuffle_index(size);
        if (conf->party_id == conf->sp_id)
        {
            std::iota(shuffle_index.begin(), shuffle_index.end(), 0);
            std::random_device rd;
            std::mt19937 mt(rd());
            std::shuffle(shuffle_index.begin(), shuffle_index.end(), mt);
            for (int i = 0; i < num; ++i)
            {
                s_index[i] += FixedPoint(shuffle_index[i]);
            }
        }
        open(s_index);
        auto common_index_ = recons(s_index);
        std::vector<int> ret;
        std::transform(
            common_index_.begin(),
            common_index_.end(),
            std::back_inserter(ret),
            [](const FixedPoint i) { return static_cast<int>(i.getDoubleVal()); }
        );
        return ret;
    }
};
}  // namespace qmpc::ObjectiveFunction