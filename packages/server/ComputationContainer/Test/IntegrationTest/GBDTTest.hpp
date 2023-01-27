#pragma once

#include <iomanip>
#include <vector>

#include "GBDT/SGBM.hpp"
#include "GBDT/SID3.hpp"
#include "GBDT/SID3Regression.hpp"
#include "Logging/Logger.hpp"
#include "gtest/gtest.h"

auto bitVecCreater = [](const std::vector<std::vector<int>> &data)
{
    int col = std::size(data[0]);

    std::vector<std::vector<std::vector<FixedPoint>>> fixed_vec;
    for (int i = 0; i < col; ++i)
    {
        std::map<int, int> col_value;
        int index{};
        for (const auto &cur : data)
        {
            col_value[cur[i]] = -1;
        }
        for (auto &[_, value] : col_value)
        {
            static_cast<void>(_);  // NOTE: unused warningを消すため
            value = index++;
        }
        int value_size = std::size(col_value);
        int t_size = std::size(data);
        std::vector<std::vector<FixedPoint>> bit_data(value_size, std::vector<FixedPoint>(t_size));
        for (int index = 0; index < t_size; ++index)
        {
            bit_data[col_value[data[index][i]]][index] = 1;
        }
        fixed_vec.emplace_back(bit_data);
    }

    int attribute_size = fixed_vec.size();
    std::vector<std::vector<std::vector<Share>>> x(attribute_size);
    for (int i = 0; i < attribute_size; ++i)
    {
        int attribute_value_size = fixed_vec[i].size();
        std::vector<std::vector<Share>> xx(attribute_value_size);
        for (int j = 0; j < attribute_value_size; ++j)
        {
            int data_size = fixed_vec[i][j].size();
            std::vector<Share> bit_vector(data_size);
            for (int k = 0; k < data_size; ++k)
            {
                bit_vector[k] = qmpc::Share::getConstantShare(fixed_vec[i][j][k]);
            }
            xx[j] = bit_vector;
        }
        x[i] = xx;
    }
    return x;
};
TEST(SID3Test, sid3test)
{
    //データ作成
    std::vector<std::vector<std::vector<FixedPoint>>> x_ = {
        {{0, 0, 0, 0, 1}, {1, 1, 0, 0, 0}, {0, 0, 1, 1, 0}},
        {{1, 1, 1, 1, 0}, {0, 0, 0, 0, 1}},
        {{0, 0, 0, 1, 1}, {1, 1, 1, 0, 0}}};

    int attribute_size = x_.size();
    std::vector<std::vector<std::vector<Share>>> x(attribute_size);
    for (int i = 0; i < attribute_size; ++i)
    {
        int attribute_value_size = x_[i].size();
        std::vector<std::vector<Share>> xx(attribute_value_size);
        for (int j = 0; j < attribute_value_size; ++j)
        {
            int data_size = x_[i][j].size();
            std::vector<Share> bit_vector(data_size);
            for (int k = 0; k < data_size; ++k)
            {
                bit_vector[k] = qmpc::Share::getConstantShare(x_[i][j][k]);
            }
            xx[j] = bit_vector;
        }
        x[i] = xx;
    }
    std::vector<std::vector<FixedPoint>> t_ = {{1, 1, 0, 1, 0}, {0, 0, 1, 0, 0}, {0, 0, 0, 0, 1}};
    std::vector<std::vector<Share>> t(t_.size(), std::vector<Share>(t_[0].size()));
    for (size_t i = 0; i < t_.size(); ++i)
    {
        for (size_t j = 0; j < t_[i].size(); ++j)
        {
            t[i][j] = qmpc::Share::getConstantShare(t_[i][j]);
        }
    }
    std::vector<Share> T(t[0].size());
    for (auto &t : T)
    {
        t += FixedPoint(1);
    }
    std::set<int> R;
    for (int i = 0; i < attribute_size; ++i)
    {
        R.insert(i);
    }
    auto tree = qmpc::GBDT::SID3::createTree(T, x, t, R);
    QMPC_LOG_INFO("create tree end");
    QMPC_LOG_INFO("{}", tree->createTreeJson());

    // predict test
    std::vector<std::vector<FixedPoint>> test_data1 = {{0, 1, 0}, {1, 0}, {0, 1}};
    std::vector<std::vector<FixedPoint>> test_data2 = {{0, 0, 1}, {1, 0}, {0, 1}};
    auto convertToShare = [](const std::vector<std::vector<FixedPoint>> &test_data)
    {
        std::vector<std::vector<Share>> stest_data;
        for (size_t i = 0; i < test_data.size(); ++i)
        {
            std::vector<Share> test_(test_data[i].size());
            for (size_t j = 0; j < test_data[i].size(); ++j)
            {
                test_[j] = qmpc::Share::getConstantShare(test_data[i][j]);
            }
            stest_data.emplace_back(test_);
        }
        return stest_data;
    };

    QMPC_LOG_INFO("tree predict1 start ");
    auto target1 = tree->predict(convertToShare(test_data1));
    open(target1);
    auto rec1 = recons(target1);
    EXPECT_EQ(rec1, FixedPoint("0.0"));

    QMPC_LOG_INFO("tree predict2 start ");
    auto target = tree->predict(convertToShare(test_data2));
    open(target);
    auto rec = recons(target);
    EXPECT_EQ(rec, FixedPoint("1.0"));
}

TEST(SID3RegTest, categoryError)
{  //データ作成
    std::vector<std::vector<std::vector<FixedPoint>>> x_ = {
        {{0, 0, 0, 0, 1}, {1, 1, 0, 0, 0}, {0, 0, 1, 1, 0}},
        {{1, 1, 1, 1, 0}, {0, 0, 0, 0, 1}},
        {{0, 0, 0, 1, 1}, {1, 1, 1, 0, 0}}};

    int attribute_size = x_.size();
    std::vector<std::vector<std::vector<Share>>> x(attribute_size);
    for (int i = 0; i < attribute_size; ++i)
    {
        int attribute_value_size = x_[i].size();
        std::vector<std::vector<Share>> xx(attribute_value_size);
        for (int j = 0; j < attribute_value_size; ++j)
        {
            int data_size = x_[i][j].size();
            std::vector<Share> bit_vector(data_size);
            for (int k = 0; k < data_size; ++k)
            {
                bit_vector[k] = qmpc::Share::getConstantShare(x_[i][j][k]);
            }
            xx[j] = bit_vector;
        }
        x[i] = xx;
    }
    std::vector<Share> t = {
        FixedPoint(3), FixedPoint(5), FixedPoint(10), FixedPoint(0.5), FixedPoint(3.2)};
    std::vector<Share> T(t.size());
    for (auto &tt : T)
    {
        tt += FixedPoint(1);
    }
    std::set<int> R;
    for (int i = 0; i < attribute_size; ++i)
    {
        R.insert(i);
    }
    auto tree = qmpc::GBDT::SID3Regression::createTree(T, x, t, R);
    QMPC_LOG_INFO("{}", tree->createTreeJson());

    // predict test
    std::vector<std::vector<FixedPoint>> test_data1 = {{0, 1, 0}, {1, 0}, {0, 1}};
    std::vector<std::vector<FixedPoint>> test_data2 = {{0, 0, 1}, {1, 0}, {0, 1}};
    std::vector<std::vector<FixedPoint>> test_data3 = {{1, 0, 0}, {0, 1}, {1, 0}};

    auto convertToShare = [](const std::vector<std::vector<FixedPoint>> &test_data)
    {
        std::vector<std::vector<Share>> stest_data;
        for (size_t i = 0; i < test_data.size(); ++i)
        {
            std::vector<Share> test_(test_data[i].size());
            for (size_t j = 0; j < test_data[i].size(); ++j)
            {
                test_[j] = qmpc::Share::getConstantShare(test_data[i][j]);
            }
            stest_data.emplace_back(test_);
        }
        return stest_data;
    };

    QMPC_LOG_INFO("tree predict1 start ");
    auto target1 = tree->predict(convertToShare(test_data1));
    open(target1);
    auto rec1 = recons(target1);
    QMPC_LOG_INFO("1 test predict ");

    QMPC_LOG_INFO("tree predict2 start ");
    auto target = tree->predict(convertToShare(test_data2));
    open(target);
    auto rec = recons(target);
    QMPC_LOG_INFO("2 test predict {}", rec);

    QMPC_LOG_INFO("tree predict3 start ");
    auto target3 = tree->predict(convertToShare(test_data3));
    open(target3);
    auto rec3 = recons(target3);
    QMPC_LOG_INFO("3 test predict {}", rec3);
}

TEST(SGBMTest, treeGradTest)
{
    //データ作成
    std::vector<std::vector<int>> test_d = {
        {2, 1, 2},
        {2, 1, 2},
        {3, 1, 2},
        {3, 1, 1},
        {1, 2, 1},
        {2, 1, 2},
        {1, 2, 2},
        {1, 1, 1},
        {3, 1, 1},
        {3, 2, 1}};
    auto x = bitVecCreater(test_d);
    std::vector<Share> t = {
        FixedPoint(3),
        FixedPoint(5),
        FixedPoint(10),
        FixedPoint(0.5),
        FixedPoint(3.2),
        FixedPoint(3),
        FixedPoint(5),
        FixedPoint(1),
        FixedPoint(20),
        FixedPoint(15),
    };
    auto ave = qmpc::Math::smean(t);
    for (auto &&tt : t)
    {
        tt -= ave;
    }
    open(t);
    auto t_rec = recons(t);
    for (auto &a : t_rec)
    {
        QMPC_LOG_INFO("t is {}", a);
    }
    std::vector<Share> T(t.size());
    for (auto &tt : T)
    {
        tt += FixedPoint(1);
    }
    std::set<int> R;
    for (size_t i = 0; i < x.size(); ++i)
    {
        R.insert(i);
    }
    auto tree = qmpc::GBDT::SID3Regression::createTree(T, x, t, R);
    QMPC_LOG_INFO("{}", tree->createTreeJson());
    auto tree_grads = tree->grad();
    open(tree_grads);
    auto grads = recons(tree_grads);
    for (size_t i = 0; i < T.size(); ++i)
    {
        QMPC_LOG_INFO("grad {} is {}", i, grads[i]);
    }
    // predict test
    std::vector<std::vector<FixedPoint>> test_data1 = {{0, 1, 0}, {1, 0}, {0, 1}};
    std::vector<std::vector<FixedPoint>> test_data2 = {{0, 0, 1}, {1, 0}, {0, 1}};
    std::vector<std::vector<FixedPoint>> test_data3 = {{1, 0, 0}, {0, 1}, {1, 0}};

    auto convertToShare = [](const std::vector<std::vector<FixedPoint>> &test_data)
    {
        std::vector<std::vector<Share>> stest_data;
        for (size_t i = 0; i < test_data.size(); ++i)
        {
            std::vector<Share> test_(test_data[i].size());
            for (size_t j = 0; j < test_data[i].size(); ++j)
            {
                test_[j] = qmpc::Share::getConstantShare(test_data[i][j]);
            }
            stest_data.emplace_back(test_);
        }
        return stest_data;
    };

    QMPC_LOG_INFO("tree predict1 start ");
    auto target1 = tree->predict(convertToShare(test_data1));
    open(target1);
    auto rec1 = recons(target1);
    QMPC_LOG_INFO("1 test predict {}", rec1);

    QMPC_LOG_INFO("tree predict2 start ");
    auto target = tree->predict(convertToShare(test_data2));
    open(target);
    auto rec = recons(target);
    QMPC_LOG_INFO("2 test predict {}", rec);

    QMPC_LOG_INFO("tree predict3 start ");
    auto target3 = tree->predict(convertToShare(test_data3));
    open(target3);
    auto rec3 = recons(target3);
    QMPC_LOG_INFO("3 test predict {}", rec3);
}

TEST(SGBMTest, GBDTRegressionTest)
{
    std::vector<std::vector<int>> test_d = {
        {2, 1, 2},
        {2, 1, 2},
        {3, 2, 2},
        {4, 1, 1},
        {1, 2, 1},
        {2, 1, 2},
        {1, 2, 2},
        {4, 1, 1},
        {3, 2, 2},
        {3, 2, 1},
        {4, 2, 1}};
    auto x = bitVecCreater(test_d);
    std::vector<Share> t = {
        FixedPoint(3),
        FixedPoint(5),
        FixedPoint(10),
        FixedPoint(0.5),
        FixedPoint(3.2),
        FixedPoint(3),
        FixedPoint(5),
        FixedPoint(1),
        FixedPoint(20),
        FixedPoint(15),
        FixedPoint(2)};
    open(t);
    auto t_rec = recons(t);
    std::vector<Share> T(t.size());
    for (auto &tt : T)
    {
        tt += FixedPoint(1);
    }
    std::set<int> R;
    for (size_t i = 0; i < x.size(); ++i)
    {
        R.insert(i);
    }
    auto gt = qmpc::GBDT::SGBM(0.1, T, x, t, R);
    auto pred = gt.boosting(20);
    open(pred);
    auto p_rec = recons(pred);
    for (size_t i = 0; i < t.size(); ++i)
    {
        QMPC_LOG_INFO("pred is {}", p_rec[i]);
        QMPC_LOG_INFO("t is {}", t_rec[i]);
    }
    // for (const auto &json : gt.getJson())
    // {
    //     QMPC_LOG_INFO("tree");
    //     QMPC_LOG_INFO("{}",json);
    // }

    // predict test
    std::vector<std::vector<FixedPoint>> test_data1 = {{1, 0, 0, 0}, {1, 0}, {0, 1}};
    std::vector<std::vector<FixedPoint>> test_data2 = {{0, 1, 0, 0}, {0, 1}, {0, 1}};
    std::vector<std::vector<FixedPoint>> test_data3 = {{0, 0, 0, 1}, {0, 1}, {1, 0}};
    std::vector<std::vector<FixedPoint>> test_data4 = {{0, 0, 1, 0}, {0, 1}, {0, 1}};

    auto convertToShare = [](const std::vector<std::vector<FixedPoint>> &test_data)
    {
        std::vector<std::vector<Share>> stest_data;
        for (size_t i = 0; i < test_data.size(); ++i)
        {
            std::vector<Share> test_(test_data[i].size());
            for (size_t j = 0; j < test_data[i].size(); ++j)
            {
                test_[j] = qmpc::Share::getConstantShare(test_data[i][j]);
            }
            stest_data.emplace_back(test_);
        }
        return stest_data;
    };
    auto target1 = gt.predict(convertToShare(test_data1));
    open(target1);
    auto rec1 = recons(target1);
    QMPC_LOG_INFO("1 test predict {}", rec1);
    auto target = gt.predict(convertToShare(test_data2));
    open(target);
    auto rec = recons(target);
    QMPC_LOG_INFO("2 test predict {}", rec);
    auto target3 = gt.predict(convertToShare(test_data3));
    open(target3);
    auto rec3 = recons(target3);
    QMPC_LOG_INFO("3 test predict {}", rec3);

    auto target4 = gt.predict(convertToShare(test_data4));
    open(target4);
    auto rec4 = recons(target4);
    QMPC_LOG_INFO("4 test predict {}", rec4);
}

TEST(SID3Test, sid3JsonConstructorTest)
{
    // TODO: データ作成部を集約させる
    //データ作成
    std::vector<std::vector<std::vector<FixedPoint>>> x_ = {
        {{0, 0, 0, 0, 1}, {1, 1, 0, 0, 0}, {0, 0, 1, 1, 0}},
        {{1, 1, 1, 1, 0}, {0, 0, 0, 0, 1}},
        {{0, 0, 0, 1, 1}, {1, 1, 1, 0, 0}}};

    int attribute_size = x_.size();
    std::vector<std::vector<std::vector<Share>>> x(attribute_size);
    for (int i = 0; i < attribute_size; ++i)
    {
        int attribute_value_size = x_[i].size();
        std::vector<std::vector<Share>> xx(attribute_value_size);
        for (int j = 0; j < attribute_value_size; ++j)
        {
            int data_size = x_[i][j].size();
            std::vector<Share> bit_vector(data_size);
            for (int k = 0; k < data_size; ++k)
            {
                bit_vector[k] = qmpc::Share::getConstantShare(x_[i][j][k]);
            }
            xx[j] = bit_vector;
        }
        x[i] = xx;
    }

    std::vector<std::vector<FixedPoint>> t_ = {{1, 1, 0, 1, 0}, {0, 0, 1, 0, 0}, {0, 0, 0, 0, 1}};
    std::vector<std::vector<Share>> t(t_.size(), std::vector<Share>(t_[0].size()));
    for (size_t i = 0; i < t_.size(); ++i)
    {
        for (size_t j = 0; j < t_[i].size(); ++j)
        {
            t[i][j] = qmpc::Share::getConstantShare(t_[i][j]);
        }
    }
    std::vector<Share> T(t[0].size());
    for (auto &t : T)
    {
        t += FixedPoint(1);
    }
    std::set<int> R;
    for (int i = 0; i < attribute_size; ++i)
    {
        R.insert(i);
    }
    auto tree = qmpc::GBDT::SID3::createTree(T, x, t, R);
    auto tree_json = tree->createTreeJson();
    auto tree2 = std::make_unique<qmpc::GBDT::SID3>(tree_json);
    auto tree2_json = tree2->createTreeJson();

    EXPECT_EQ(tree_json, tree2_json);
}

TEST(SID3Test, sid3JsonPredictTest)
{
    // TODO: データ作成部を集約させる
    //データ作成
    std::vector<std::vector<std::vector<FixedPoint>>> x_ = {
        {{0, 0, 0, 0, 1}, {1, 1, 0, 0, 0}, {0, 0, 1, 1, 0}},
        {{1, 1, 1, 1, 0}, {0, 0, 0, 0, 1}},
        {{0, 0, 0, 1, 1}, {1, 1, 1, 0, 0}}};

    int attribute_size = x_.size();
    std::vector<std::vector<std::vector<Share>>> x(attribute_size);
    for (int i = 0; i < attribute_size; ++i)
    {
        int attribute_value_size = x_[i].size();
        std::vector<std::vector<Share>> xx(attribute_value_size);
        for (int j = 0; j < attribute_value_size; ++j)
        {
            int data_size = x_[i][j].size();
            std::vector<Share> bit_vector(data_size);
            for (int k = 0; k < data_size; ++k)
            {
                bit_vector[k] = qmpc::Share::getConstantShare(x_[i][j][k]);
            }
            xx[j] = bit_vector;
        }
        x[i] = xx;
    }

    std::vector<std::vector<FixedPoint>> t_ = {{1, 1, 0, 1, 0}, {0, 0, 1, 0, 0}, {0, 0, 0, 0, 1}};
    std::vector<std::vector<Share>> t(t_.size(), std::vector<Share>(t_[0].size()));
    for (size_t i = 0; i < t_.size(); ++i)
    {
        for (size_t j = 0; j < t_[i].size(); ++j)
        {
            t[i][j] = qmpc::Share::getConstantShare(t_[i][j]);
        }
    }
    std::vector<Share> T(t[0].size());
    for (auto &t : T)
    {
        t += FixedPoint(1);
    }
    std::set<int> R;
    for (int i = 0; i < attribute_size; ++i)
    {
        R.insert(i);
    }
    auto tree = qmpc::GBDT::SID3::createTree(T, x, t, R);
    QMPC_LOG_INFO("create tree end ");
    QMPC_LOG_INFO("{}", tree->createTreeJson());

    // predict test
    std::vector<std::vector<FixedPoint>> test_data1 = {{0, 1, 0}, {1, 0}, {0, 1}};
    std::vector<std::vector<FixedPoint>> test_data2 = {{0, 0, 1}, {1, 0}, {0, 1}};
    auto tree_json = tree->createTreeJson();
    auto tree2 = std::make_unique<qmpc::GBDT::SID3>(tree_json);
    auto convertToShare = [](const std::vector<std::vector<FixedPoint>> &test_data)
    {
        std::vector<std::vector<Share>> stest_data;
        for (size_t i = 0; i < test_data.size(); ++i)
        {
            std::vector<Share> test_(test_data[i].size());
            for (size_t j = 0; j < test_data[i].size(); ++j)
            {
                test_[j] = qmpc::Share::getConstantShare(test_data[i][j]);
            }
            stest_data.emplace_back(test_);
        }
        return stest_data;
    };

    QMPC_LOG_INFO("tree predict1 start ");
    auto target1 = tree->predict(convertToShare(test_data1));
    auto target2 = tree2->predict(convertToShare(test_data1));
    open(target1);
    auto rec1 = recons(target1);
    open(target2);
    auto rec2 = recons(target2);
    EXPECT_EQ(rec1, rec2);

    QMPC_LOG_INFO("tree predict2 start ");
    auto target3 = tree->predict(convertToShare(test_data2));
    auto target4 = tree2->predict(convertToShare(test_data2));
    open(target3);
    auto rec3 = recons(target3);
    open(target4);
    auto rec4 = recons(target4);
    EXPECT_EQ(rec3, rec4);
}
