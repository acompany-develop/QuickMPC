#pragma once
#include <vector>

#include "Job/Jobs/LogisticRegressionJob.hpp"
#include "Model/Models/DecisionTree.hpp"
#include "Model/Models/LinearRegression.hpp"
#include "Model/Models/LogisticRegression.hpp"
#include "Optimizer/GradientDescent.hpp"
#include "Share/Share.hpp"
#include "external/Proto/ManageToComputationContainer/manage_to_computation.grpc.pb.h"
#include "gtest/gtest.h"

TEST(ModelTest, LinearRegressionTest)
{
    Config *conf = Config::getInstance();
    int n_parties = conf->n_parties;
    auto w = std::vector<Share>{FixedPoint("1"), FixedPoint("2"), FixedPoint("3")};
    auto x = std::vector<Share>{FixedPoint("2"), FixedPoint("4"), FixedPoint("8")};
    auto model = qmpc::Model::LinearRegression();
    auto val_s = model.predict_f(w, x);

    qmpc::Share::open(val_s);
    auto val = qmpc::Share::recons(val_s);

    // (1*2 + 2*4 + 3*8) * n_parties^2 = 34 * n_parties^2
    EXPECT_EQ(val, FixedPoint(std::to_string(34 * n_parties * n_parties)));
}

TEST(ModelTest, LogisticRegressionTest)
{
    Config *conf = Config::getInstance();
    int n_parties = conf->n_parties;
    auto w = std::vector<Share>{FixedPoint("1"), FixedPoint("2"), FixedPoint("3")};
    auto x = std::vector<Share>{FixedPoint("2"), FixedPoint("4"), FixedPoint("8")};
    auto model = qmpc::Model::LogisticRegression();
    auto val_s = model.predict_f(w, x);

    qmpc::Share::open(val_s);
    auto val = qmpc::Share::recons(val_s);

    auto true_val = 1.0 / (1.0 + std::exp(-34 * n_parties * n_parties));
    EXPECT_NEAR(val.getDoubleVal(), true_val, 0.01);
}

TEST(ModelTest, DecisionTreeTest)
{
    Config *conf = Config::getInstance();
    int n_parties = conf->n_parties;
    nlohmann::json dtree_list{
        {{"weight", "0.3"},
         {"tree_structure",
          {{"feature_bitvector", {"0", "0", std::to_string(1.0 / n_parties)}},
           {"threshold", "0.5"},
           {"left_child",
            {
                {"output", "0"},
            }},
           {"right_child",
            {
                {"output", "1"},
            }}}}},
        {{"weight", "0.7"},
         {"tree_structure",
          {{"feature_bitvector", {std::to_string(1.0 / n_parties), "0", "0"}},
           {"threshold", "0.7"},
           {"left_child",
            {
                {"output", "0"},
            }},
           {"right_child",
            {
                {"output", "1"},
            }}}}}};
    auto model = qmpc::Model::DecisionTree();

    std::vector<Share> x1 = {FixedPoint("0.6"), FixedPoint("0.3"), FixedPoint("0.7")};
    auto ds1 = model.predict_f(dtree_list, x1);
    open(ds1);
    auto d1 = recons(ds1);
    EXPECT_NEAR(d1.getDoubleVal(), (0.3 * n_parties * n_parties) + (0.7 * n_parties * 0), 0.001);

    std::vector<Share> x2 = {FixedPoint("0.1"), FixedPoint("0.1"), FixedPoint("0.1")};
    auto ds2 = model.predict_f(dtree_list, x2);
    open(ds2);
    auto d2 = recons(ds2);
    EXPECT_NEAR(d2.getDoubleVal(), (0.3 * n_parties * 0) + (2.1 * n_parties * 0), 0.001);

    std::vector<Share> x3 = {FixedPoint("1.0"), FixedPoint("1.0"), FixedPoint("1.0")};
    auto ds3 = model.predict_f(dtree_list, x3);
    open(ds3);
    auto d3 = recons(ds3);
    EXPECT_NEAR(
        d3.getDoubleVal(), (0.3 * n_parties * n_parties) + (0.7 * n_parties * n_parties), 0.001
    );
}

/**
// NOTE: 精度計測Testは時間がかかる上に精度を測るだけなのでコメント化しておく
auto logistic(std::vector<std::vector<int>> &x_fp, std::vector<int> &y_fp)
{
    // 値をShareに変換する
    int h = x_fp.size();
    int w = x_fp[0].size();
    std::vector<std::vector<Share>> x(h, std::vector<Share>(w));
    std::vector<Share> y(h);
    for (int i = 0; i < h; ++i)
    {
        y[i] += FixedPoint(y_fp[i]);
        for (int j = 0; j < w; ++j)
        {
            x[i][j] += FixedPoint(x_fp[i][j]);
        }
    }

    // パラメータを探索する
    auto f = qmpc::Job::LogisticLikelihoodFunction(x, y);
    auto opt = qmpc::Optimizer::GradientDescent(FixedPoint("0.1"));
    auto theta = opt.optimize(100, w, f, false);

    // パラメータを用いて推論して正解率を出力する
    auto model = qmpc::Model::LogisticRegression();
    int cnt = 0;
    for (int i = 0; i < h; ++i)
    {
        auto val_s = model.predict_f(theta, x[i]);

        qmpc::Share::open(val_s);
        auto val = qmpc::Share::recons(val_s);

        auto val_01 = (val < 0.5) ? 0 : 1;

        cnt += (val_01 == y[i]);
    }
    spdlog::info("ロジスティック回帰 正解率: {} ({} / {})",1.0 * cnt / h,cnt,h);
}

TEST(ModelTest, LogisticGradientDescentTest)
{
    // small case
    std::vector<std::vector<int>> x_fp1{
        {{1, 1, 2, 3},
         {1, 1, 2, 2},
         {1, 2, 5, 7},
         {1, 3, 1, 1},
         {1, 5, 2, 4}}};
    std::vector<int> y_fp1{
        {1, 1, 0, 1, 0}};
    logistic(x_fp1, y_fp1);

    // medium case
    std::vector<std::vector<int>> x_fp2{
        {{1, 1, 2, 3, 2, 1, 2},
         {1, 1, 2, 2, 2, 1, 2},
         {1, 2, 5, 7, 1, 1, 6},
         {1, 3, 1, 1, 5, 4, 3},
         {1, 2, 2, 4, 1, 2, 3},
         {2, 3, 1, 1, 1, 2, 3},
         {3, 4, 2, 2, 3, 2, 1},
         {4, 5, 1, 3, 4, 5, 6}}};
    std::vector<int> y_fp2{
        {1, 1, 0, 1, 0, 1, 1, 0}};
    logistic(x_fp2, y_fp2);

    // large case
    std::vector<std::vector<int>> x_fp3{
        {{1, 2, 2, 3, 2, 1, 2, 1, 3},
         {1, 3, 1, 2, 2, 5, 4, 2, 4},
         {1, 4, 2, 1, 1, 2, 6, 3, 6},
         {1, 5, 1, 1, 5, 4, 3, 6, 4},
         {1, 6, 7, 4, 2, 4, 3, 3, 2},
         {2, 3, 1, 4, 5, 2, 3, 1, 1},
         {3, 4, 6, 2, 3, 2, 1, 6, 1},
         {2, 3, 5, 1, 1, 2, 3, 4, 4},
         {3, 1, 2, 8, 3, 2, 1, 5, 3},
         {1, 2, 4, 2, 2, 1, 2, 6, 1},
         {1, 3, 5, 3, 6, 1, 6, 2, 2},
         {1, 4, 2, 1, 5, 4, 3, 1, 4},
         {1, 2, 3, 2, 5, 2, 3, 4, 6},
         {1, 1, 1, 2, 1, 2, 3, 2, 1},
         {3, 2, 2, 1, 2, 2, 1, 4, 1},
         {4, 3, 1, 3, 4, 5, 6, 8, 5}}};
    std::vector<int> y_fp3{
        {1, 1, 0, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0, 1, 0}};
    logistic(x_fp3, y_fp3);
}
/*/
/**/