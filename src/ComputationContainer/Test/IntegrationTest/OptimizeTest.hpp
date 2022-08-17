#pragma once

#include <iomanip>
#include <vector>

#include "LogHeader/Logger.hpp"
#include "ObjectiveFunction/ObjectiveFunctionInterface.hpp"
#include "Optimizer/optimizer.hpp"
#include "gtest/gtest.h"

TEST(OptimizeTest, GradientDescent)
{
    class QuadraticFunction : public qmpc::ObjectiveFunction::ObjectiveFunctionInterface
    {
        using Share = ::Share;

    public:
        std::vector<::Share> df(const std::vector<Share> &theta) const override
        {
            return std::vector<Share>{theta[0] * 2.0 + 2.0, theta[1] * 6.0};
        }
        std::vector<::Share> df(int num, const std::vector<Share> &theta) const override
        {
            return std::vector<Share>{theta[0] * 2.0 + 2.0, theta[1] * 6.0};
        }
    };

    std::vector<Share> zero(2);
    auto opt = qmpc::Optimizer::GradientDescent(FixedPoint("0.01"));
    auto f = QuadraticFunction();
    constexpr int iterateNum = 200;
    auto theta = opt.optimize(iterateNum, f, zero);
    /*
     * y = (x1)^2 + 3(x2)^2 + 2(x1) + 3
     * y'= { 2(x1) + 2 , 6(x2) }
     * min = 2 ( x1 = -1, x2 = 0 )
     */
    open(theta);
    auto theta_rec = recons(theta);
    EXPECT_NEAR(theta_rec[0].getDoubleVal(), -1.0, 0.1);
    EXPECT_NEAR(theta_rec[1].getDoubleVal(), 0.0, 0.1);
}

TEST(OptimizeTest, optimizer)
{
    //データ作成
    std::vector<std::vector<FixedPoint>> x_ = {
        {1, 2, 3}, {1, 2, 2}, {2, 5, 7}, {3, 1, 1}, {5, 2, 4}};

    int data_size = x_.size();
    std::vector<std::vector<Share>> x(data_size, std::vector<Share>(x_[0].size() + 1));
    for (size_t i = 0; i < x_.size(); ++i)
    {
        x[i][0] = qmpc::Share::getConstantShare(FixedPoint(1));
        for (size_t j = 0; j < x_[0].size(); ++j)
        {
            x[i][j + 1] = qmpc::Share::getConstantShare(x_[i][j]);
        }
    }
    std::vector<FixedPoint> t_ = {1, 1, 0, 1, 0};
    std::vector<Share> t(t_.size());
    for (size_t i = 0; i < t_.size(); ++i) t[i] = qmpc::Share::getConstantShare(t_[i]);

    //目的関数作成
    qmpc::ObjectiveFunction::CrossEntropyError function(x, t);

    //最適化オブジェクト生成
    std::vector<std::pair<std::string, std::shared_ptr<qmpc::Optimizer::OptInterface>>> opts;
    // 探索のハイパーパラメータ，現在は0.1
    // 最急降下法でパラメータ最適化
    auto opt1 =
        new qmpc::Optimizer::GradientDescent(qmpc::Share::getConstantShare(FixedPoint("1")));
    opts.emplace_back(std::string("gd"), opt1);
    // 確率的勾配降下方
    int batch_size = std::size(x) / 2;
    auto opt2 =
        new qmpc::Optimizer::SGD(qmpc::Share::getConstantShare(FixedPoint("1")), batch_size);
    opts.emplace_back(std::string("sgd"), opt2);
    // auto opt3 = new qmpc::Optimizer::NewTon();
    // opts.emplace_back(std::string("newton"), opt3);

    auto opt4 = new qmpc::Optimizer::Momentum(FixedPoint("0.75"), FixedPoint("0.1"), batch_size);
    opts.emplace_back(std::string("momentum"), opt4);

    auto opt5 = new qmpc::Optimizer::Adam();
    opts.emplace_back(std::string("adam"), opt5);

    //ロジスティック回帰の重み
    std::vector<Share> theta(x[0].size());
    // 繰り返し回数
    // TODO:入力で変更するようにした方が良い
    int iterateNum = 100;

    for (auto &&[name, opt] : opts)
    {
        spdlog::info("start opt is", name);
        std::vector<Share> ret;
        // if (name == "newton")
        //     iterateNum = 10;
        auto start = std::chrono::system_clock::now();
        ret = opt->optimize(iterateNum, function, theta);
        auto end = std::chrono::system_clock::now();
        auto dur = end - start;  // 要した時間を計算
        auto msec = std::chrono::duration_cast<std::chrono::milliseconds>(dur).count();
        // 要した時間をミリ秒（1/1000秒）に変換して表示
        spdlog::info("{} milli sec", msec);
        open(t);
        auto t_rec = recons(t);
        //係数
        open(ret);
        auto ret_rec = recons(ret);
        for (size_t i = 0; i < ret.size(); ++i)
        {
            spdlog::info("theta {} {}", i, ret_rec[i]);
        }
        std::vector<Share> y(data_size);
        for (int i = 0; i < data_size; ++i)
        {
            y[i] = qmpc::Math::open_sigmoid_vector(x[i] * ret);
        }
        open(y);
        auto y_rec = recons(y);

        double accuracy = 0.0;
        for (int i = 0; i < data_size; ++i)
        {
            double diff = std::abs((t_rec[i] - y_rec[i]).getDoubleVal());
            if (diff < 0.5)
            {
                accuracy++;
            }
        }
        spdlog::info("{} accuracy: {:.10f}", name, accuracy / data_size);
#ifdef DEBUG

        for (auto a : t_rec)
        {
            spdlog::info("t is {}", a);
        }
        for (auto a : y_rec)
        {
            spdlog::info("y is {}", a);
        }
#endif
    }
}