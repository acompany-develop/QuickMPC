#pragma once
#include <cmath>
#include <random>
#include <vector>

#include "gtest/gtest.h"
#include "logging/logger.hpp"
#include "math/math.hpp"

TEST(MathTest, Smean)
{
    Share a(FixedPoint("1.0"));
    Share b(FixedPoint("2.0"));
    Share c(FixedPoint("3.0"));
    std::vector<Share> v(3);
    v[0] = a;
    v[1] = b;
    v[2] = c;

    Share avg;
    avg = qmpc::Math::smean(v);
    FixedPoint avg_rec = open_and_recons(avg);

    Config *conf = Config::getInstance();
    int n_parties = conf->n_parties;
    FixedPoint expected(std::to_string((1.0 + 3.0) / 2.0 * n_parties));

    QMPC_LOG_INFO("avg_rec: {}", avg_rec.getStrVal());
    EXPECT_NEAR(expected.getDoubleVal(), avg_rec.getDoubleVal(), 0.001);
}

TEST(MathTest, Smean2)
{
    int n = 30000;
    std::vector<Share> v(n);

    for (int i = 0; i < n; i++)
    {
        Share a(FixedPoint((long long int)i + 1));
        v[i] = a;
    }

    Share avg;
    avg = qmpc::Math::smean(v);
    FixedPoint avg_rec = open_and_recons(avg);

    Config *conf = Config::getInstance();
    int n_parties = conf->n_parties;
    FixedPoint expected(std::to_string((1.0 + 30000.0) / 2.0 * n_parties));

    QMPC_LOG_INFO("avg_rec: {}", avg_rec.getStrVal());
    EXPECT_NEAR(expected.getDoubleVal(), avg_rec.getDoubleVal(), 0.001);
}

TEST(MathTest, Variance)
{
    Share a(FixedPoint("1.0"));
    Share b(FixedPoint("2.0"));
    Share c(FixedPoint("3.0"));
    std::vector<Share> v(3);
    v[0] = a;
    v[1] = b;
    v[2] = c;

    Share var;
    var = qmpc::Math::variance(v);
    FixedPoint var_rec = open_and_recons(var);

    Config *conf = Config::getInstance();
    int n_parties = conf->n_parties;
    std::vector<double> v_double = {1.0, 2.0, 3.0};
    for (size_t i = 0; i < v_double.size(); i++)
    {
        v_double[i] = v_double[i] * n_parties;
    }
    double avg = (v_double.front() + v_double.back()) / 2.0;
    double vari_sum = 0.0;
    for (size_t i = 0; i < v_double.size(); i++)
    {
        vari_sum += (v_double[i] - avg) * (v_double[i] - avg);
    }
    FixedPoint expected(std::to_string(vari_sum / v_double.size()));

    QMPC_LOG_INFO("var_rec: {}", var_rec.getStrVal());
    EXPECT_NEAR(expected.getDoubleVal(), var_rec.getDoubleVal(), 0.001);
}

TEST(MathTest, Variance2)
{
    int n = 30000;
    std::vector<Share> v(n);
    for (int i = 0; i < n; i++)
    {
        Share a(FixedPoint((long long int)i + 1));
        v[i] = a;
    }

    Share var;
    var = qmpc::Math::variance(v);
    FixedPoint var_rec = open_and_recons(var);

    Config *conf = Config::getInstance();
    int n_parties = conf->n_parties;
    std::vector<int> v_double(n);
    for (int i = 0; i < n; i++)
    {
        v_double[i] = i * n_parties;
    }
    double avg = (v_double.front() + v_double.back()) / 2.0;
    double vari_sum = 0.0;
    for (int i = 0; i < n; i++)
    {
        vari_sum += (v_double[i] - avg) * (v_double[i] - avg);
    }
    FixedPoint expected(std::to_string(vari_sum / v_double.size()));

    QMPC_LOG_INFO("var_rec: {}", var_rec.getStrVal());
    EXPECT_NEAR(expected.getDoubleVal(), var_rec.getDoubleVal(), 0.001);
}

TEST(MathTest, Stdev)
{
    Share a(FixedPoint("1.0"));
    Share b(FixedPoint("2.0"));
    Share c(FixedPoint("3.0"));
    std::vector<Share> v(3);
    v[0] = a;
    v[1] = b;
    v[2] = c;
    FixedPoint target = qmpc::Math::stdev(v);

    Config *conf = Config::getInstance();
    int n_parties = conf->n_parties;
    std::vector<double> v_double = {1.0, 2.0, 3.0};
    for (size_t i = 0; i < v_double.size(); i++)
    {
        v_double[i] = v_double[i] * n_parties;
    }
    double avg = (v_double.front() + v_double.back()) / 2.0;
    double vari_sum = 0.0;
    for (size_t i = 0; i < v_double.size(); i++)
    {
        vari_sum += (v_double[i] - avg) * (v_double[i] - avg);
    }
    double dev = std::sqrt(vari_sum / v_double.size());
    FixedPoint expected(std::to_string(dev));

    EXPECT_NEAR(expected.getDoubleVal(), target.getDoubleVal(), 0.01);
}

TEST(MathTest, Correl)
{
    std::vector<Share> x = {FixedPoint("1.0"), FixedPoint("2.0"), FixedPoint("5.0")};
    std::vector<Share> y = {FixedPoint("10.0"), FixedPoint("5.0"), FixedPoint("6.0")};
    Share correl_rec = qmpc::Math::correl(x, y);
    FixedPoint target = open_and_recons(correl_rec);

    FixedPoint expected("-0.54470478");

    EXPECT_NEAR(expected.getDoubleVal(), target.getDoubleVal(), 0.01);
}

TEST(MathTest, Correl_0div)
{
    std::vector<Share> x = {FixedPoint("2.0"), FixedPoint("2.0"), FixedPoint("2.0")};
    std::vector<Share> y = {FixedPoint("10.0"), FixedPoint("10.0"), FixedPoint("10.0")};

    Share correl_rec = qmpc::Math::correl(x, y);
    FixedPoint target = open_and_recons(correl_rec);
    EXPECT_NEAR(target.getDoubleVal(), 0.0, 0.001);
}

TEST(MathTest, Correl_size)
{
    std::vector<Share> x = {FixedPoint("2.0"), FixedPoint("2.0")};
    std::vector<Share> y = {FixedPoint("10.0"), FixedPoint("5.0"), FixedPoint("6.0")};

    try
    {
        qmpc::Math::correl(x, y);
    }
    catch (std::exception e)
    {
        QMPC_LOG_INFO(e.what());
        EXPECT_TRUE(true);
        QMPC_LOG_INFO("TestCorrel_size \033[32m Succeed \033[m");
        return;
    }
    EXPECT_TRUE(false);
    QMPC_LOG_INFO("TestCorrel_size \033[32m Fail \033[m");
}

// 28000個の要素を持つベクトル2つの相関係数の計算時間測定
TEST(MathTest, Correl_large)
{
    constexpr int MIN = 0;
    constexpr int MAX = 1;
    constexpr int N = 28000;

    std::vector<Share> x(N);
    std::vector<Share> y(N);

    std::random_device rd;
    std::default_random_engine eng(rd());
    std::uniform_real_distribution<double> distr(MIN, MAX);

    for (int i = 0; i < N; ++i)
    {
        x[i] = FixedPoint(distr(eng));
        y[i] = FixedPoint(distr(eng));
    }

    auto start = std::chrono::system_clock::now();

    Share correl = qmpc::Math::correl(x, y);
    FixedPoint correl_rec = open_and_recons(correl);

    auto end = std::chrono::system_clock::now();
    auto dur = end - start;

    // 計算に要した時間をミリ秒（1/1000秒）に変換して表示
    auto msec = std::chrono::duration_cast<std::chrono::milliseconds>(dur).count();

    QMPC_LOG_INFO("28000個の要素を持つベクトル2つの相関係数の計算時間: {} milli sec", msec);
    QMPC_LOG_INFO(correl_rec.getStrVal());
}

TEST(MathTest, ExpTest)
{
    Config *conf = Config::getInstance();
    int n_parties = conf->n_parties;

    // x = n_parties;
    Share x{1};
    auto start = std::chrono::system_clock::now();

    auto exp_n = qmpc::Math::exp(x);
    double expect = std::exp(n_parties);
    auto exp_n_rec = open_and_recons(exp_n);
    auto end = std::chrono::system_clock::now();
    auto dur = end - start;

    // 計算に要した時間をミリ秒（1/1000秒）に変換して表示
    auto msec = std::chrono::duration_cast<std::chrono::milliseconds>(dur).count();

    QMPC_LOG_INFO("share exp time is {}", msec);
    QMPC_LOG_INFO("share exp_n is {}", exp_n_rec);
    QMPC_LOG_INFO("expect exp_n is {}", expect);

    EXPECT_NEAR(expect, exp_n_rec.getDoubleVal(), 0.001);
}
TEST(MathTest, correlVecExceptionTest)
{
    constexpr int N = 28000;

    std::vector<Share> x(N);
    std::vector<Share> y(N);
    y.pop_back();
    try
    {
        Share correl = qmpc::Math::correl(x, y);
    }
    catch (std::exception &e)
    {
        QMPC_LOG_ERROR("{} | {}", *boost::get_error_info<qmpc::Log::traced>(e), e.what());
    }
}