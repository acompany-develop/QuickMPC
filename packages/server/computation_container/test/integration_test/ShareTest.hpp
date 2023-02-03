#pragma once
#include <algorithm>
#include <iostream>
#include <random>
#include <string>
#include <thread>

#include "Logging/Logger.hpp"
#include "random/random.hpp"
#include "Server/ComputationToComputationContainer/Server.hpp"
#include "Share/Compare.hpp"
#include "Share/Share.hpp"
#include "gtest/gtest.h"
#include "unistd.h"

TEST(ShareTest, IncrementShareId)
{
    Share a(FixedPoint("3.0"));
    Share b(FixedPoint("3.0"));
    int diff = b.getId().getShareId() - a.getId().getShareId();
    EXPECT_EQ(diff, 1);
}

TEST(ShareTest, GetShareValue)
{
    Share a(FixedPoint("3.0"));
    EXPECT_EQ(a.getVal(), FixedPoint("3.0"));
}

TEST(ShareTest, AddBetweenShares)
{
    Config *conf = Config::getInstance();
    int n_parties = conf->n_parties;
    Share a(FixedPoint("1.0"));
    Share b(FixedPoint("2.0"));
    a = a + b;
    open(a);
    FixedPoint a_rec = recons(a);
    EXPECT_EQ(a_rec, FixedPoint(std::to_string(3.0 * n_parties)));
}

TEST(ShareTest, SubBetweenShares)
{
    Config *conf = Config::getInstance();
    int n_parties = conf->n_parties;
    Share a(FixedPoint("2.0"));
    Share b(FixedPoint("1.0"));
    a = a - b;
    open(a);
    FixedPoint a_rec = recons(a);
    EXPECT_EQ(a_rec, FixedPoint(std::to_string(n_parties)));
}

TEST(ShareTest, MulBetweenShares)
{
    Config *conf = Config::getInstance();
    int n_parties = conf->n_parties;
    Share a(FixedPoint("3.0"));
    Share b(FixedPoint("3.0"));
    a = a * b;
    open(a);
    FixedPoint a_rec = recons(a);
    QMPC_LOG_INFO("a_rec = {}", a_rec.getDoubleVal());
    EXPECT_EQ(a_rec, FixedPoint(std::to_string((3.0 * n_parties) * (3.0 * n_parties))));
}

TEST(ShareTest, DivBetweenShares)
{
    Share a(FixedPoint("1.0"));
    Share b(FixedPoint("2.0"));
    a = a / b;
    open(a);
    FixedPoint a_rec = recons(a);
    QMPC_LOG_INFO("a_rec = {}", a_rec.getDoubleVal());
    EXPECT_NEAR(a_rec.getDoubleVal(), 0.50, 0.01);
}

TEST(ShareTest, AddBetweenShareAndFixedPoint)
{
    Config *conf = Config::getInstance();
    int n_parties = conf->n_parties;
    Share a(FixedPoint("1.0"));
    FixedPoint b("2.0");
    a = a + b;
    open(a);
    FixedPoint a_rec = recons(a);
    EXPECT_EQ(a_rec, FixedPoint(std::to_string(n_parties + 2.0)));
}

TEST(ShareTest, SubBetweenShareAndFixedPoint)
{
    Config *conf = Config::getInstance();
    int n_parties = conf->n_parties;
    Share a(FixedPoint("10.0"));
    FixedPoint b("2.0");
    a = a - b;
    open(a);
    FixedPoint a_rec = recons(a);
    EXPECT_EQ(a_rec, FixedPoint(std::to_string(10.0 * n_parties - 2.0)));
}

TEST(ShareTest, MulBetweenShareAndFixedPoint)
{
    Config *conf = Config::getInstance();
    int n_parties = conf->n_parties;
    Share a(FixedPoint("2.0"));
    FixedPoint b("3.0");
    a = a * b;
    open(a);
    FixedPoint a_rec = recons(a);
    EXPECT_EQ(a_rec, FixedPoint(std::to_string(2.0 * n_parties * 3.0)));
}

TEST(ShareTest, DivBetweenShareAndFixedPoint)
{
    Config *conf = Config::getInstance();
    int n_parties = conf->n_parties;
    Share a(FixedPoint("1.0"));
    FixedPoint b("2.0");
    a = a / b;
    open(a);
    FixedPoint a_rec = recons(a);
    EXPECT_EQ(a_rec, FixedPoint(std::to_string(n_parties / 2.0)));
}

// 各要素の加法に関する逆元を一括で求めるテスト
TEST(ShareTest, GetAdditiveInvVec)
{
    Config *conf = Config::getInstance();
    int n_parties = conf->n_parties;
    std::vector<Share> a = {
        Share(FixedPoint("5")),
        Share(FixedPoint("3.6")),
        Share(FixedPoint("-6")),
        Share(FixedPoint("-4.2")),
        Share(FixedPoint("0"))};
    a = getAdditiveInvVec(a);
    std::vector<FixedPoint> expected = {
        (-5.0 * n_parties), (-3.6 * n_parties), (6.0 * n_parties), (4.2 * n_parties), 0};
    open(a);
    std::vector<FixedPoint> ret = recons(a);
    for (int i = 0; i < static_cast<int>(a.size()); ++i)
    {
        EXPECT_EQ(expected[i], ret[i].getDoubleVal());
    }
}

// 各要素の乗法に関する逆元を一括で求めるテスト
TEST(ShareTest, GetMultiplicativeInvVec)
{
    Config *conf = Config::getInstance();
    int n_parties = conf->n_parties;
    std::vector<Share> a = {
        Share(FixedPoint("5")),
        Share(FixedPoint("3.6")),
        Share(FixedPoint("-6")),
        Share(FixedPoint("-4.2"))};
    a = getMultiplicativeInvVec(a);
    std::vector<double> expected = {
        1 / (5.0 * n_parties),
        1 / (3.6 * n_parties),
        -1 / (6.0 * n_parties),
        -1 / (4.2 * n_parties)};
    open(a);
    std::vector<FixedPoint> ret = recons(a);
    for (int i = 0; i < static_cast<int>(a.size()); ++i)
    {
        EXPECT_NEAR(expected[i], ret[i].getDoubleVal(), 0.1);
    }
}

TEST(ShareTest, AddBetweenSharesAndFixedPoint)
{
    Config *conf = Config::getInstance();
    int n_parties = conf->n_parties;
    std::vector<Share> a = {
        Share(FixedPoint("5")),
        Share(FixedPoint("3.6")),
        Share(FixedPoint("-6")),
        Share(FixedPoint("-4.2"))};
    FixedPoint b("2.0");
    std::vector<double> expected = {
        5.0 * n_parties + 2.0,
        3.6 * n_parties + 2.0,
        -6.0 * n_parties + 2.0,
        -4.2 * n_parties + 2.0};
    std::vector<Share> c = a + b;
    open(c);
    std::vector<FixedPoint> ret = recons(c);
    for (int i = 0; i < static_cast<int>(a.size()); ++i)
    {
        EXPECT_NEAR(expected[i], ret[i].getDoubleVal(), 0.1);
    }

    c = b + a;
    open(c);
    ret = recons(c);
    for (int i = 0; i < static_cast<int>(a.size()); ++i)
    {
        EXPECT_NEAR(expected[i], ret[i].getDoubleVal(), 0.1);
    }
}

TEST(ShareTest, SubBetweenSharesAndFixedPoint)
{
    Config *conf = Config::getInstance();
    int n_parties = conf->n_parties;
    std::vector<Share> a = {
        Share(FixedPoint("5")),
        Share(FixedPoint("3.6")),
        Share(FixedPoint("-6")),
        Share(FixedPoint("-4.2"))};
    FixedPoint b("2.0");
    std::vector<double> expected = {
        5.0 * n_parties - 2.0,
        3.6 * n_parties - 2.0,
        -6.0 * n_parties - 2.0,
        -4.2 * n_parties - 2.0};
    std::vector<Share> c = a - b;
    open(c);
    std::vector<FixedPoint> ret = recons(c);
    for (int i = 0; i < static_cast<int>(a.size()); ++i)
    {
        EXPECT_NEAR(expected[i], ret[i].getDoubleVal(), 0.1);
    }

    c = b - a;
    open(c);
    ret = recons(c);
    for (int i = 0; i < static_cast<int>(a.size()); ++i)
    {
        EXPECT_NEAR(-expected[i], ret[i].getDoubleVal(), 0.1);
    }
}

TEST(ShareTest, MulBetweenSharesAndFixedPoint)
{
    Config *conf = Config::getInstance();
    int n_parties = conf->n_parties;
    std::vector<Share> a = {
        Share(FixedPoint("5")),
        Share(FixedPoint("3.6")),
        Share(FixedPoint("-6")),
        Share(FixedPoint("-4.2"))};
    FixedPoint b("2.0");
    std::vector<double> expected = {
        5.0 * n_parties * 2.0,
        3.6 * n_parties * 2.0,
        -6.0 * n_parties * 2.0,
        -4.2 * n_parties * 2.0};
    std::vector<Share> c = a * b;
    open(c);
    std::vector<FixedPoint> ret = recons(c);
    for (int i = 0; i < static_cast<int>(a.size()); ++i)
    {
        EXPECT_NEAR(expected[i], ret[i].getDoubleVal(), 0.1);
    }

    c = b * a;
    open(c);
    ret = recons(c);
    for (int i = 0; i < static_cast<int>(a.size()); ++i)
    {
        EXPECT_NEAR(expected[i], ret[i].getDoubleVal(), 0.1);
    }
}

TEST(ShareTest, DivBetweenSharesAndFixedPoint)
{
    Config *conf = Config::getInstance();
    int n_parties = conf->n_parties;
    std::vector<Share> a = {
        Share(FixedPoint("5")),
        Share(FixedPoint("3.6")),
        Share(FixedPoint("-6")),
        Share(FixedPoint("-4.2"))};
    FixedPoint b("2.0");
    std::vector<double> expected = {
        5.0 * n_parties / 2.0,
        3.6 * n_parties / 2.0,
        -6.0 * n_parties / 2.0,
        -4.2 * n_parties / 2.0};
    std::vector<Share> c = a / b;
    open(c);
    std::vector<FixedPoint> ret = recons(c);
    for (int i = 0; i < static_cast<int>(a.size()); ++i)
    {
        EXPECT_NEAR(expected[i], ret[i].getDoubleVal(), 0.1);
    }

    c = b / a;
    open(c);
    ret = recons(c);
    for (int i = 0; i < static_cast<int>(a.size()); ++i)
    {
        EXPECT_NEAR(1.0 / expected[i], ret[i].getDoubleVal(), 0.1);
    }
}

TEST(ShareTest, ModuloBetweenShareAndFixedPoint)
{
    Config *conf = Config::getInstance();
    int n_parties = conf->n_parties;

    // テスト1
    Share a(FixedPoint("6.0"));
    FixedPoint b("7.0");
    FixedPoint ans = a % b;
    QMPC_LOG_INFO("ans = {}", ans.getStrVal());
    EXPECT_EQ(ans, FixedPoint(std::to_string(6 * n_parties % 7)));

    // テスト2
    a = Share(FixedPoint("-15.0"));
    b = FixedPoint("11.0");
    ans = a % b;
    QMPC_LOG_INFO("ans = {}", ans.getStrVal());
    EXPECT_EQ(ans, FixedPoint(std::to_string(((-15 * n_parties % 11) + 11) % 11)));

    // テスト3
    a = Share(FixedPoint("17.0"));
    b = FixedPoint("-4.0");
    ans = a % b;
    QMPC_LOG_INFO("ans = {}", ans.getStrVal());
    EXPECT_EQ(ans, FixedPoint(std::to_string(((17 * n_parties % -4) - 4) % -4)));

    // テスト4
    a = Share(FixedPoint("-13.0"));
    b = FixedPoint("-6.0");
    ans = a % b;
    QMPC_LOG_INFO("ans = {}", ans.getStrVal());
    EXPECT_EQ(ans, FixedPoint(std::to_string(-13 * n_parties % -6)));

    // テスト5
    a = Share(FixedPoint("10453.0"));
    b = FixedPoint("129.0");
    ans = a % b;
    QMPC_LOG_INFO("ans = {}", ans.getStrVal());
    EXPECT_EQ(ans, FixedPoint(std::to_string(10453 * n_parties % 129)));

    // テスト6
    a = Share(FixedPoint("-33914.0"));
    b = FixedPoint("529.0");
    ans = a % b;
    QMPC_LOG_INFO("ans = {}", ans.getStrVal());
    EXPECT_EQ(ans, FixedPoint(std::to_string(((-33914 * n_parties % 529) + 529) % 529)));

    // テスト7
    a = Share(FixedPoint("20991.0"));
    b = FixedPoint("-1034.0");
    ans = a % b;
    QMPC_LOG_INFO("ans = {}", ans.getStrVal());
    EXPECT_EQ(ans, FixedPoint(std::to_string(((20991 * n_parties % -1034) - 1034) % -1034)));

    // テスト8
    a = Share(FixedPoint("-18001.0"));
    b = FixedPoint("-4034.0");
    ans = a % b;
    QMPC_LOG_INFO("ans = {}", ans.getStrVal());
    EXPECT_EQ(ans, FixedPoint(std::to_string(-18001 * n_parties % -4034)));

    // テスト9
    a = Share(FixedPoint("7535565.0"));
    b = FixedPoint("9617262.0");
    ans = a % b;
    QMPC_LOG_INFO("ans = {}", ans.getStrVal());
    EXPECT_EQ(ans, FixedPoint(std::to_string(7535565 * n_parties % 9617262)));

    // テスト10
    a = Share(FixedPoint("-1190027.0"));
    b = FixedPoint("3492693.0");
    ans = a % b;
    QMPC_LOG_INFO("ans = {}", ans.getStrVal());
    EXPECT_EQ(
        ans, FixedPoint(std::to_string(((-1190027 * n_parties % 3492693) + 3492693) % 3492693))
    );

    // テスト11
    a = Share(FixedPoint("2957085.0"));
    b = FixedPoint("-15625920.0");
    ans = a % b;
    QMPC_LOG_INFO("ans = {}", ans.getStrVal());
    EXPECT_EQ(
        ans, FixedPoint(std::to_string(((2957085 * n_parties % -15625920) - 15625920) % -15625920))
    );

    // テスト12
    a = Share(FixedPoint("-11866897.0"));
    b = FixedPoint("-15090045.0");
    ans = a % b;
    QMPC_LOG_INFO("ans = {}", ans.getStrVal());
    EXPECT_EQ(ans, FixedPoint(std::to_string(-11866897 * n_parties % -15090045)));

    // テスト13
    a = Share(FixedPoint("478935003.0"));
    b = FixedPoint("315492017.0");
    ans = a % b;
    QMPC_LOG_INFO("ans = {}", ans.getStrVal());
    EXPECT_EQ(ans, FixedPoint(std::to_string(478935003 * n_parties % 315492017)));

    // テスト14
    a = Share(FixedPoint("-430333161.0"));
    b = FixedPoint("683330992.0");
    ans = a % b;
    QMPC_LOG_INFO("ans = {}", ans.getStrVal());
    EXPECT_EQ(
        ans,
        FixedPoint(std::to_string(((-430333161 * n_parties % 683330992) + 683330992) % 683330992))
    );

    // テスト15
    a = Share(FixedPoint("784585987.0"));
    b = FixedPoint("-311360387.0");
    ans = a % b;
    QMPC_LOG_INFO("ans = {}", ans.getStrVal());
    EXPECT_EQ(
        ans,
        FixedPoint(std::to_string(((784585987LL * n_parties % -311360387) - 311360387) % -311360387)
        )
    );

    // テスト16
    a = Share(FixedPoint("-427544319.0"));
    b = FixedPoint("-818477560.0");
    ans = a % b;
    QMPC_LOG_INFO("ans = {}", ans.getStrVal());
    EXPECT_EQ(ans, FixedPoint(std::to_string(-427544319 * n_parties % -818477560)));
}

TEST(ShareTest, AddBetweenFixedPointAndShare)
{
    Config *conf = Config::getInstance();
    int n_parties = conf->n_parties;
    Share a(FixedPoint("1.0"));
    FixedPoint b("2.0");
    a = b + a;
    open(a);
    FixedPoint a_rec = recons(a);
    EXPECT_EQ(a_rec, FixedPoint(std::to_string(2 + n_parties)));
}

TEST(ShareTest, SubBetweenFixedPointAndShare)
{
    Config *conf = Config::getInstance();
    int n_parties = conf->n_parties;
    Share a(FixedPoint("2.0"));
    FixedPoint b("10.0");
    a = b - a;
    open(a);
    FixedPoint a_rec = recons(a);
    EXPECT_EQ(a_rec, FixedPoint(std::to_string(10 - (2 * n_parties))));
}

TEST(ShareTest, MulBetweenFixedPointAndShare)
{
    Config *conf = Config::getInstance();
    int n_parties = conf->n_parties;
    Share a(FixedPoint("2.0"));
    FixedPoint b("3.0");
    a = b * a;
    open(a);
    FixedPoint a_rec = recons(a);
    EXPECT_EQ(a_rec, FixedPoint(std::to_string(3 * (2 * n_parties))));
}

TEST(ShareTest, DivBetweenFixedPointAndShare)
{
    Config *conf = Config::getInstance();
    int n_parties = conf->n_parties;
    Share a(FixedPoint("2.0"));
    FixedPoint b("12.0");
    a = b / a;
    open(a);
    FixedPoint a_rec = recons(a);
    QMPC_LOG_INFO("a_rec = {}", a_rec.getDoubleVal());
    EXPECT_NEAR(a_rec.getDoubleVal(), 12.0 / (2 * n_parties), 0.1);
}

TEST(ShareTest, RandBitShare)
{
    int N = 5;
    for (int i = 0; i < N; ++i)
    {
        Share a = qmpc::Share::getRandBitShare<FixedPoint>();
        open(a);
        FixedPoint a_rec = recons(a);
        QMPC_LOG_INFO("RandBit = {}", a_rec.getDoubleVal());
        bool left = (-0.01 < a_rec.getDoubleVal()) && (a_rec.getDoubleVal() < 0.01);
        bool right = (0.99 < a_rec.getDoubleVal()) && (a_rec.getDoubleVal() < 1.01);
        EXPECT_TRUE(left || right) << "a_rec = " << a_rec;
    }
}

// 一括RandBitShareのテスト
TEST(ShareTest, BulkRandBitShare)
{
    int N = 5;
    std::vector<Share> a = qmpc::Share::getRandBitShare<FixedPoint>(N);
    open(a);
    std::vector<FixedPoint> a_rec = recons(a);
    for (int i = 0; i < N; ++i)
    {
        QMPC_LOG_INFO("RandBit = {}", a_rec[i].getDoubleVal());
        bool left = (-0.01 < a_rec[i].getDoubleVal()) && (a_rec[i].getDoubleVal() < 0.01);
        bool right = (0.99 < a_rec[i].getDoubleVal()) && (a_rec[i].getDoubleVal() < 1.01);
        EXPECT_TRUE(left || right) << "a_rec = " << a_rec[i];
    }
}

TEST(ShareTest, LSBShare)
{
    Config *conf = Config::getInstance();
    int n_parties = conf->n_parties;
    std::vector<Share> s = {
        Share(FixedPoint("5")),
        Share(FixedPoint("6")),
        Share(FixedPoint("7.1")),
        Share(FixedPoint("7.4"))};
    std::vector<std::vector<double>> expected = {
        {5.0 * n_parties, (double)(5 * n_parties % 2)},
        {6.0 * n_parties, (double)(6 * n_parties % 2)},
        {7.1 * n_parties, fmod(round(7.1 * n_parties), 2)},
        {7.4 * n_parties, fmod(round(7.4 * n_parties), 2)}};
    double error = 0.0001;
    for (int i = 0; i < static_cast<int>(s.size()); ++i)
    {
        Share lsb = qmpc::Share::getLSBShare(s[i]);
        open(lsb);
        FixedPoint lsb_rec = recons(lsb);
        QMPC_LOG_INFO("LSB({}) = {}", expected[i][0], lsb_rec.getDoubleVal());
        EXPECT_NEAR(expected[i][1], lsb_rec.getDoubleVal(), error);
    }
}

// 一括LSBShareのテスト
TEST(ShareTest, BulkLSBShare)
{
    Config *conf = Config::getInstance();
    int n_parties = conf->n_parties;
    std::vector<Share> s = {
        Share(FixedPoint("5")),
        Share(FixedPoint("6")),
        Share(FixedPoint("7.1")),
        Share(FixedPoint("7.4"))};
    std::vector<std::vector<double>> expected = {
        {5.0 * n_parties, (double)(5 * n_parties % 2)},
        {6.0 * n_parties, (double)(6 * n_parties % 2)},
        {7.1 * n_parties, fmod(round(7.1 * n_parties), 2)},
        {7.4 * n_parties, fmod(round(7.4 * n_parties), 2)}};
    double error = 0.0001;

    std::vector<Share> lsb = qmpc::Share::getLSBShare(s);
    open(lsb);
    std::vector<FixedPoint> lsb_rec = recons(lsb);
    for (int i = 0; i < static_cast<int>(s.size()); ++i)
    {
        QMPC_LOG_INFO("LSB({}) = {}", expected[i][0], lsb_rec[i].getDoubleVal());
        EXPECT_NEAR(expected[i][1], lsb_rec[i].getDoubleVal(), error);
    }
}

TEST(ShareTest, Floor)
{
    Config *conf = Config::getInstance();
    int n_parties = conf->n_parties;
    std::vector<Share> s = {
        Share(FixedPoint("3.5")),
        Share(FixedPoint("9.26")),
        Share(FixedPoint("4.6666")),
        Share(FixedPoint("4.6667")),
        Share(FixedPoint("3.0")),
        Share(FixedPoint("-3.5")),
        Share(FixedPoint("-9.26")),
        Share(FixedPoint("-4.6666")),
        Share(FixedPoint("-4.6667")),
        Share(FixedPoint("-3.0")),
    };

    // [(floor 未適用の値, floor 適用した値)]
    std::vector<std::vector<double>> expected = {
        {3.5 * n_parties, floor(3.5 * n_parties)},
        {9.26 * n_parties, floor(9.26 * n_parties)},
        {4.6666 * n_parties, floor(4.6666 * n_parties)},
        {4.6667 * n_parties, floor(4.6667 * n_parties)},
        {3.0 * n_parties, floor(3.0 * n_parties)},
        {-3.5 * n_parties, floor(-3.5 * n_parties)},
        {-9.26 * n_parties, floor(-9.26 * n_parties)},
        {-4.6666 * n_parties, floor(-4.6666 * n_parties)},
        {-4.6667 * n_parties, floor(-4.6667 * n_parties)},
        {-3.0 * n_parties, floor(-3.0 * n_parties)},
    };
    double error = 0.0001;
    for (int i = 0; i < static_cast<int>(s.size()); ++i)
    {
        Share s_floor = qmpc::Share::getFloor(s[i]);
        open(s_floor);
        FixedPoint result = recons(s_floor);
        QMPC_LOG_INFO("floor({}) = {}", expected[i][0], result);
        EXPECT_NEAR(expected[i][1], result.getDoubleVal(), error);
    }
}

// 一括Floorのテスト
TEST(ShareTest, BulkFloor)
{
    Config *conf = Config::getInstance();
    int n_parties = conf->n_parties;
    std::vector<Share> s = {
        Share(FixedPoint("3.5")),
        Share(FixedPoint("9.26")),
        Share(FixedPoint("4.6666")),
        Share(FixedPoint("4.6667")),
        Share(FixedPoint("3.0")),
        Share(FixedPoint("-3.5")),
        Share(FixedPoint("-9.26")),
        Share(FixedPoint("-4.6666")),
        Share(FixedPoint("-4.6667")),
        Share(FixedPoint("-3.0")),
    };

    // [(floor 未適用の値, floor 適用した値)]
    std::vector<std::vector<double>> expected = {
        {3.5 * n_parties, floor(3.5 * n_parties)},
        {9.26 * n_parties, floor(9.26 * n_parties)},
        {4.6666 * n_parties, floor(4.6666 * n_parties)},
        {4.6667 * n_parties, floor(4.6667 * n_parties)},
        {3.0 * n_parties, floor(3.0 * n_parties)},
        {-3.5 * n_parties, floor(-3.5 * n_parties)},
        {-9.26 * n_parties, floor(-9.26 * n_parties)},
        {-4.6666 * n_parties, floor(-4.6666 * n_parties)},
        {-4.6667 * n_parties, floor(-4.6667 * n_parties)},
        {-3.0 * n_parties, floor(-3.0 * n_parties)}};
    double error = 0.0001;

    std::vector<Share> s_floor = qmpc::Share::getFloor(s);
    open(s_floor);
    std::vector<FixedPoint> result = recons(s_floor);
    for (int i = 0; i < static_cast<int>(s.size()); ++i)
    {
        QMPC_LOG_INFO("floor({}) = {}", expected[i][0], result[i]);
        EXPECT_NEAR(expected[i][1], result[i].getDoubleVal(), error);
    }
}

// LTZ (Less Than Zero)
TEST(ShareTest, LTZ)
{
    Config *conf = Config::getInstance();
    int n_parties = conf->n_parties;
    std::vector<Share> s = {
        Share(FixedPoint("3.0")),
        Share(FixedPoint("-3.0")),
        Share(FixedPoint("10.5")),
        Share(FixedPoint("-10.5")),
        Share(FixedPoint("10250.4")),
        Share(FixedPoint("-10250.4")),
        Share(FixedPoint("0.0")),
        Share(FixedPoint("0.01")),
        Share(FixedPoint("0.0001")),
        Share(FixedPoint("-0.01")),
        Share(FixedPoint("-0.0001")),
    };

    // [(真値, LTZ の結果)]
    std::vector<std::vector<double>> expected = {
        {3.0 * n_parties, 0},
        {-3.0 * n_parties, 1},
        {10.5 * n_parties, 0},
        {-10.5 * n_parties, 1},
        {10250.4 * n_parties, 0},
        {-10250.4 * n_parties, 1},
        {0.0, 0},
        {0.01 * n_parties, 0},
        {0.0001 * n_parties, 0},
        {-0.01 * n_parties, 1},
        {-0.0001 * n_parties, 1},
    };
    double error = 0.00001;
    for (int i = 0; i < static_cast<int>(s.size()); ++i)
    {
        Share s_ltz = qmpc::Share::LTZ(s[i]);
        open(s_ltz);
        FixedPoint result = recons(s_ltz);
        QMPC_LOG_INFO("[{}<0] {}", expected[i][0], result);
        EXPECT_NEAR(result.getDoubleVal(), expected[i][1], error);
    }
}

TEST(ShareTest, LTZBulk)
{
    Config *conf = Config::getInstance();
    int n_parties = conf->n_parties;
    std::vector<Share> s = {
        Share(FixedPoint("3.0")),
        Share(FixedPoint("-3.0")),
        Share(FixedPoint("10.5")),
        Share(FixedPoint("-10.5")),
        Share(FixedPoint("10250.4")),
        Share(FixedPoint("-10250.4")),
        Share(FixedPoint("0.0")),
        Share(FixedPoint("0.01")),
        Share(FixedPoint("0.0001")),
        Share(FixedPoint("-0.01")),
        Share(FixedPoint("-0.0001")),
    };

    // [(真値, LTZ の結果)]
    std::vector<std::vector<double>> expected = {
        {3.0 * n_parties, 0},
        {-3.0 * n_parties, 1},
        {10.5 * n_parties, 0},
        {-10.5 * n_parties, 1},
        {10250.4 * n_parties, 0},
        {-10250.4 * n_parties, 1},
        {0.0, 0},
        {0.01 * n_parties, 0},
        {0.0001 * n_parties, 0},
        {-0.01 * n_parties, 1},
        {-0.0001 * n_parties, 1},
    };
    double error = 0.00001;
    auto s_ltz = qmpc::Share::LTZ(s);
    open(s_ltz);
    auto result = recons(s_ltz);
    for (int i = 0; i < static_cast<int>(s.size()); ++i)
    {
        QMPC_LOG_INFO("[{}<0] {}", expected[i][0], result[i]);
        EXPECT_NEAR(result[i].getDoubleVal(), expected[i][1], error);
    }
}

// Share(とFixedPoint)での比較が可能かテストする
TEST(ShareTest, ComparisonOperation)
{
    Config *conf = Config::getInstance();
    int n_parties = conf->n_parties;
    Share a(FixedPoint("2.0"));
    Share b(FixedPoint("3.0"));
    Share c(FixedPoint("3.0"));
    FixedPoint d(std::to_string(3.0 * n_parties));

    EXPECT_TRUE(a < b);
    EXPECT_TRUE(!(a > b));
    EXPECT_TRUE(a <= b);
    EXPECT_TRUE(!(a >= b));
    EXPECT_TRUE(a != b);
    EXPECT_TRUE(b == c);
    EXPECT_TRUE(a < d);
    EXPECT_TRUE(!(a > d));
    EXPECT_TRUE(a <= d);
    EXPECT_TRUE(!(a >= d));
    EXPECT_TRUE(c == d);
    EXPECT_TRUE(c <= d);
    EXPECT_TRUE(c >= d);
    EXPECT_TRUE(d > a);
    EXPECT_TRUE(!(d < a));
    EXPECT_TRUE(d >= a);
    EXPECT_TRUE(!(d <= a));
    EXPECT_TRUE(d != a);
    EXPECT_TRUE(d == c);
    EXPECT_TRUE(d <= c);
    EXPECT_TRUE(d >= c);
}

TEST(ShareTest, ComparisonOperationBulk)
{
    Share a(FixedPoint("2.0"));
    Share b(FixedPoint("3.0"));
    std::vector<Share> l{a, a, b, b};
    std::vector<Share> r{a, b, a, b};

    // <
    auto lt = allLess(l, r);
    std::vector<bool> lt_t{false, true, false, false};
    EXPECT_EQ(lt, lt_t);

    // >
    auto gt = allGreater(l, r);
    std::vector<bool> gt_t{false, false, true, false};
    EXPECT_EQ(gt, gt_t);

    // <=
    auto lte = allLessEq(l, r);
    std::vector<bool> lte_t{true, true, false, true};
    EXPECT_EQ(lte, lte_t);

    // >=
    auto gte = allGreaterEq(l, r);
    std::vector<bool> gte_t{true, false, true, true};
    EXPECT_EQ(gte, gte_t);

    // ==
    auto eq = allEq(l, r);
    std::vector<bool> eq_t{true, false, false, true};
    EXPECT_EQ(eq, eq_t);
}

TEST(ShareTest, EqualityEpsilonRandomTest)
{
    Config *conf = Config::getInstance();
    int n_parties = conf->n_parties;

    // m,k is LTZ parameters
    int m = 20;
    int k = 48;
    std::vector<std::pair<long long, long long>> random_range{
        {1, 10},                             // small case
        {1LL << (k - 1), (1LL << k) - 20}};  // large case
    for (const auto &[lower, upper] : random_range)
    {
        for (int _ = 0; _ < 10; ++_)
        {
            auto val1 = RandGenerator::getInstance()->getRand<long long>(lower, upper);
            auto val2 = val1 + 1;

            auto val_d1 = static_cast<double>(val1) / (1LL << (m - 3)) / n_parties;
            auto val_d2 = static_cast<double>(val2) / (1LL << (m - 3)) / n_parties;

            auto s1 = Share(FixedPoint((boost::format("%.10f") % val_d1).str()));
            auto s2 = Share(FixedPoint((boost::format("%.10f") % val_d2).str()));

            EXPECT_TRUE(s1 == s1);
            EXPECT_FALSE(s1 == s2);
        }
    }
}

// ランダムな値で比較演算のテストを実行
TEST(ShareTest, RandomComparisonOperation)
{
    {
        // Share と Share の比較
        Share a_share(RandGenerator::getInstance()->getRand<FixedPoint>(-10000, 10000));
        open(a_share);
        Share b_share(RandGenerator::getInstance()->getRand<FixedPoint>(-10000, 10000));
        open(b_share);
        QMPC_LOG_INFO("a_share is {}", a_share.getVal());
        QMPC_LOG_INFO("b_share is {}", b_share.getVal());
        FixedPoint a_rec = recons(a_share);
        FixedPoint b_rec = recons(b_share);

        if (a_rec < b_rec)
        {
            EXPECT_LT(a_share, b_share);
        }
        else if (a_rec > b_rec)
        {
            EXPECT_GT(a_share, b_share);
        }
        else
        {
            EXPECT_EQ(a_share, b_share);
        }
    }
    QMPC_LOG_INFO("one step !!");
    // Share と FixedPoint の比較
    {
        Share a_share(RandGenerator::getInstance()->getRand<FixedPoint>(1, 1000));
        open(a_share);
        FixedPoint a_rec = recons(a_share);
        FixedPoint target = FixedPoint("0");
        QMPC_LOG_INFO("a_rec = \t{}", a_rec.getVal());
        QMPC_LOG_INFO("target = \t{}", target.getVal());
        if (a_rec < target)
        {
            EXPECT_LT(a_share, target);
        }
        else if (a_rec > target)
        {
            EXPECT_GT(a_share, target);
        }
        else
        {
            EXPECT_EQ(a_share, target);
        }
    }
    QMPC_LOG_INFO("two step !!");
    // FixedPoint と Share の比較
    {
        Share a_share(RandGenerator::getInstance()->getRand<FixedPoint>(1, 1000));
        open(a_share);
        FixedPoint a_rec = recons(a_share);
        FixedPoint target = FixedPoint("0");
        QMPC_LOG_INFO("a_rec = \t{}", a_rec.getVal());
        QMPC_LOG_INFO("target = \t{}", target.getVal());
        if (target < a_rec)
        {
            EXPECT_LT(target, a_share);
        }
        else if (target > a_rec)
        {
            EXPECT_GT(target, a_share);
        }
        else
        {
            EXPECT_EQ(target, a_share);
        }
    }
}

// 定数値Shareテスト
TEST(ShareTest, ConstantShare)
{
    auto fp = FixedPoint("12");
    auto s = qmpc::Share::getConstantShare(fp);
    open(s);
    auto fp_r = recons(s);
    EXPECT_EQ(fp, fp_r);
}

// 一括open,reconsテスト
TEST(ShareTest, ReconsBulk)
{
    Config *conf = Config::getInstance();
    int n_parties = conf->n_parties;
    std::vector<Share> t = {FixedPoint("1"), FixedPoint("2"), FixedPoint("3")};
    std::vector<FixedPoint> expected = {
        FixedPoint(std::to_string(n_parties)),
        FixedPoint(std::to_string(2.0 * n_parties)),
        FixedPoint(std::to_string(3.0 * n_parties))};
    open(t);
    auto target = recons(t);
    bool ng = false;

    for (int i = 0; i < static_cast<int>(t.size()); ++i)
    {
        if (expected[i] - target[i] <= FixedPoint("-0.00001")
            or expected[i] - target[i] >= FixedPoint("0.00001"))
        {
            ng = true;
        }
    }

    EXPECT_TRUE(not ng);
}

// 一括加算のテスト
TEST(ShareTest, AddBulk)
{
    Config *conf = Config::getInstance();
    int n_parties = conf->n_parties;
    std::vector<Share> a = {FixedPoint("1.0"), FixedPoint("2.0"), FixedPoint("3.0")};
    std::vector<Share> b = {FixedPoint("1.0"), FixedPoint("2.0"), FixedPoint("3.0")};
    std::vector<FixedPoint> expected = {
        FixedPoint(std::to_string(2.0 * n_parties)),
        FixedPoint(std::to_string(4.0 * n_parties)),
        FixedPoint(std::to_string(6.0 * n_parties))};
    auto ret = a + b;
    open(ret);
    auto target = recons(ret);

    QMPC_LOG_INFO("AddBulk End !!");
    EXPECT_EQ(target, expected);
}

// 一括減算のテスト
TEST(ShareTest, SubBulk)
{
    Config *conf = Config::getInstance();
    int n_parties = conf->n_parties;
    std::vector<Share> a = {FixedPoint("3.0"), FixedPoint("2.0"), FixedPoint("9.0")};
    std::vector<Share> b = {FixedPoint("1.0"), FixedPoint("6.0"), FixedPoint("3.0")};
    std::vector<FixedPoint> expected = {
        FixedPoint(std::to_string(2.0 * n_parties)),
        FixedPoint(std::to_string(-4.0 * n_parties)),
        FixedPoint(std::to_string(6.0 * n_parties))};
    auto ret = a - b;
    open(ret);
    auto target = recons(ret);

    QMPC_LOG_INFO("SubBulk End !!");
    EXPECT_EQ(target, expected);
}

// 一括乗算のテスト
TEST(ShareTest, MulBulk)
{
    Config *conf = Config::getInstance();
    int n_parties = conf->n_parties;
    std::vector<Share> a = {FixedPoint("1.0"), FixedPoint("2.0"), FixedPoint("3.0")};
    std::vector<Share> b = {FixedPoint("1.0"), FixedPoint("2.0"), FixedPoint("3.0")};
    std::vector<FixedPoint> expected = {
        FixedPoint(std::to_string(n_parties * n_parties)),
        FixedPoint(std::to_string((2.0 * n_parties) * (2.0 * n_parties))),
        FixedPoint(std::to_string((3.0 * n_parties) * (3.0 * n_parties)))};
    auto ret = a * b;
    open(ret);
    auto target = recons(ret);
    bool ng = false;

    QMPC_LOG_INFO("MulBulk End !!");
    for (int i = 0; i < static_cast<int>(a.size()); ++i)
    {
        QMPC_LOG_INFO(target[i].getStrVal());
        if (expected[i] - target[i] <= FixedPoint("-0.00001")
            or expected[i] - target[i] >= FixedPoint("0.00001"))
        {
            ng = true;
        }
    }

    EXPECT_TRUE(not ng);
}

// Share_Id 同時実行
TEST(ShareTest, SameShareId)
{
    Config *conf = Config::getInstance();
    int n_parties = conf->n_parties;
    FixedPoint ret1, ret2;
    std::thread th1(
        [&]()
        {
            qmpc::Share::AddressId::setJobId(0);
            Share a(1);
            open(a);
            ret1 = recons(a);
            QMPC_LOG_INFO("Firset thread is {}", ret1.getVal());
            QMPC_LOG_INFO("First ID is {}", a.getId());
        }
    );
    std::thread th2(
        [&]()
        {
            qmpc::Share::AddressId::setJobId(1);
            Share a(0);
            open(a);
            ret2 = recons(a);
            QMPC_LOG_INFO("Second thread is {}", ret2.getVal());
            QMPC_LOG_INFO("Second ID is {}", a.getId());
        }
    );
    th1.join();
    th2.join();

    EXPECT_EQ(ret1, FixedPoint(n_parties));
    EXPECT_EQ(ret2, FixedPoint(0));
}

TEST(ShareTest, Sort)
{
    constexpr int MIN = 0;
    constexpr int MAX = 1;
    constexpr int N = 5;

    std::vector<Share> x(N);

    std::random_device rd;
    std::default_random_engine eng(rd());
    std::uniform_real_distribution<double> distr(MIN, MAX);

    for (int i = 0; i < N; ++i)
    {
        x[i] = FixedPoint(distr(eng));
    }
    auto y = x;
    auto start = std::chrono::system_clock::now();
    std::sort(x.begin(), x.end());
    auto end = std::chrono::system_clock::now();
    auto diff = end - start;
    auto dsec = std::chrono::duration_cast<std::chrono::milliseconds>(diff).count();

    QMPC_LOG_INFO("share sorting time is : {}", dsec);
    open(y);
    auto y_rec = recons(y);

    start = std::chrono::system_clock::now();
    std::sort(y_rec.begin(), y_rec.end());

    end = std::chrono::system_clock::now();
    diff = end - start;
    dsec = std::chrono::duration_cast<std::chrono::milliseconds>(diff).count();

    QMPC_LOG_INFO("svalue sorting time is : {}", dsec);

    // for (auto &&aa : x)
    // {
    //     QMPC_LOG_INFO("{}",aa.getVal());
    // }
    open(x);
    auto x_rec = recons(x);
    for (int i = 0; i < N; ++i)
    {
        // QMPC_LOG_INFO("X is {}",x_rec[i]);
        // QMPC_LOG_INFO("Y is {}", y_rec[i]);
        EXPECT_EQ(x_rec[i], y_rec[i]);
    }
}

// sqrtのテスト
TEST(ShareTest, Sqrt)
{
    Config *conf = Config::getInstance();
    int n_parties = conf->n_parties;

    Share a(FixedPoint(3));
    auto a_sqrt = qmpc::Share::sqrt(a);
    open(a_sqrt);
    auto target = recons(a_sqrt);

    QMPC_LOG_INFO("3 * n_parties sqrt is {}", target);
    EXPECT_NEAR(target.getDoubleVal(), std::sqrt(3 * n_parties), 0.01);

    Share b;
    b += FixedPoint(121);
    auto b_sqrt = qmpc::Share::sqrt(b);
    open(b_sqrt);
    auto targetb = recons(b_sqrt);

    QMPC_LOG_INFO("121 sqrt is {}", targetb);
    EXPECT_NEAR(targetb.getDoubleVal(), 11, 0.01);
}

// streamのテスト
// 1mbを超えるデータの分割送信テスト
TEST(StreamTest, ReconsBulk)
{
    std::vector<Share> t(200000);
    std::vector<FixedPoint> expected(200000);
    for (int i = 0; i < 200000; i++)
    {
        t[i] = FixedPoint("1.0");
        expected[i] = FixedPoint("3.0");
    }
    open(t);
    auto target = recons(t);
    for (int i = 0; i < static_cast<int>(t.size()); ++i)
    {
        EXPECT_NEAR(target[i].getDoubleVal(), expected[i].getDoubleVal(), 0.00001);
    }
}
TEST(ShareTest, GenericSendShare)
{
    {
        const auto clock_start = std::chrono::system_clock::now();
        qmpc::Share::Share<bool> a{};
        open(a);
        [[maybe_unused]] auto _ = recons(a);
        const auto clock_end = std::chrono::system_clock::now();
        const auto elapsed_time_ms =
            std::chrono::duration_cast<std::chrono::microseconds>(clock_end - clock_start).count();
        QMPC_LOG_INFO("sendshare bool time = {0} ms", elapsed_time_ms);
    }
    {
        const auto clock_start = std::chrono::system_clock::now();
        qmpc::Share::Share<int> b(4);
        open(b);
        [[maybe_unused]] auto _ = recons(b);
        const auto clock_end = std::chrono::system_clock::now();
        const auto elapsed_time_ms =
            std::chrono::duration_cast<std::chrono::microseconds>(clock_end - clock_start).count();
        QMPC_LOG_INFO("sendshare int time = {0} ms", elapsed_time_ms);
    }
    {
        const auto clock_start = std::chrono::system_clock::now();
        qmpc::Share::Share<long> c(4);
        open(c);
        [[maybe_unused]] auto _ = recons(c);
        const auto clock_end = std::chrono::system_clock::now();
        const auto elapsed_time_ms =
            std::chrono::duration_cast<std::chrono::microseconds>(clock_end - clock_start).count();
        QMPC_LOG_INFO("sendshare long time = {0} ms", elapsed_time_ms);
    }
    {
        const auto clock_start = std::chrono::system_clock::now();
        qmpc::Share::Share<float> d{};
        open(d);
        [[maybe_unused]] auto _ = recons(d);
        const auto clock_end = std::chrono::system_clock::now();
        const auto elapsed_time_ms =
            std::chrono::duration_cast<std::chrono::microseconds>(clock_end - clock_start).count();
        QMPC_LOG_INFO("sendshare float time = {0} ms", elapsed_time_ms);
    }
    {
        const auto clock_start = std::chrono::system_clock::now();
        qmpc::Share::Share<double> e{};
        open(e);
        [[maybe_unused]] auto _ = recons(e);
        const auto clock_end = std::chrono::system_clock::now();
        const auto elapsed_time_ms =
            std::chrono::duration_cast<std::chrono::microseconds>(clock_end - clock_start).count();
        QMPC_LOG_INFO("sendshare double time = {0} ms", elapsed_time_ms);
    }
    {
        const auto clock_start = std::chrono::system_clock::now();
        qmpc::Share::Share<FixedPoint> f{};
        open(f);
        [[maybe_unused]] auto _ = recons(f);
        const auto clock_end = std::chrono::system_clock::now();
        const auto elapsed_time_ms =
            std::chrono::duration_cast<std::chrono::microseconds>(clock_end - clock_start).count();
        QMPC_LOG_INFO("sendshare fixedPoint time = {0} ms", elapsed_time_ms);
    }
}
TEST(ShareTest, addIntShare)
{
    Config *conf = Config::getInstance();
    int n_parties = conf->n_parties;
    qmpc::Share::Share<int> a(3);
    qmpc::Share::Share<int> b(4);
    a = a + b;  // 21
    std::cout << a.getVal() << std::endl;
    open(a);
    auto a_rec = recons(a);
    std::cout << "addint share is " << a_rec << std::endl;
    EXPECT_EQ(a_rec, n_parties * 3 + n_parties * 4);
}

TEST(ShareTest, subIntShare)
{
    Config *conf = Config::getInstance();
    int n_parties = conf->n_parties;
    qmpc::Share::Share<int> a(3);
    qmpc::Share::Share<int> b(4);
    a = a - b;
    std::cout << a.getVal() << std::endl;
    open(a);
    auto a_rec = recons(a);
    EXPECT_EQ(a_rec, n_parties * 3 - n_parties * 4);
}
TEST(ShareTest, mulIntShare)
{
    Config *conf = Config::getInstance();
    int n_parties = conf->n_parties;
    qmpc::Share::Share<int> a(3);
    qmpc::Share::Share<int> b(4);
    a = a * b;
    std::cout << a.getVal() << std::endl;
    open(a);
    auto a_rec = recons(a);
    EXPECT_EQ(a_rec, (n_parties * 3) * (n_parties * 4));
}
TEST(ShareTest, boolLarge)
{
    std::vector<qmpc::Share::Share<bool>> a(50000, true);
    open(a);
    auto target = recons(a);
}
TEST(ShareTest, IntLarge)
{
    std::vector<qmpc::Share::Share<int>> a(50000, 1);
    open(a);
    auto target = recons(a);
}
TEST(ShareTest, floatLarge)
{
    std::vector<qmpc::Share::Share<float>> a(50000, 1);
    open(a);
    auto target = recons(a);
}
TEST(ShareTest, doubleLarge)
{
    std::vector<qmpc::Share::Share<double>> a(50000, 1);
    open(a);
    auto target = recons(a);
}
TEST(ShareTest, FPLarge)
{
    std::vector<qmpc::Share::Share<FixedPoint>> a(50000, FixedPoint("1"));
    open(a);
    auto target = recons(a);
}
TEST(ShareTest, IntMulLarge)
{
    Config *conf = Config::getInstance();
    int n_parties = conf->n_parties;
    std::vector<qmpc::Share::Share<int>> a(20000, 1);
    std::vector<qmpc::Share::Share<int>> b(20000, 1);
    a = a * b;
    open(a);
    auto rec = recons(a);
    EXPECT_EQ(rec[0], (n_parties * 1) * (n_parties * 1));
}
TEST(ShareTest, FPMulLarge)
{
    Config *conf = Config::getInstance();
    int n_parties = conf->n_parties;
    std::vector<qmpc::Share::Share<FixedPoint>> a(20000, FixedPoint("1"));
    std::vector<qmpc::Share::Share<FixedPoint>> b(20000, FixedPoint("1"));
    a = a * b;
    open(a);
    auto rec = recons(a);

    EXPECT_EQ(rec[0], (n_parties * 1) * (n_parties * 1));
}

TEST(ShareTest, IntMulExtraLarge)
{
    Config *conf = Config::getInstance();
    int n_parties = conf->n_parties;
    std::vector<qmpc::Share::Share<int>> a(100000, 1);
    std::vector<qmpc::Share::Share<int>> b(100000, 1);
    a = a * b;
    open(a);
    auto rec = recons(a);
    EXPECT_EQ(rec[0], (n_parties * 1) * (n_parties * 1));
}

// 現状乱数範囲(セキュリティ)の都合上64bit浮動小数の積は使用できない
/*
TEST(ShareTest, doubleMulExtraLarge)
{
    Config *conf = Config::getInstance();
    int n_parties = conf->n_parties;
    std::vector<qmpc::Share::Share<double>> a(100000, 1.0);
    std::vector<qmpc::Share::Share<double>> b(100000, 1.0);
    a = a * b;
    open(a);
    auto rec = recons(a);
    EXPECT_EQ(rec[0], (n_parties * 1) * (n_parties * 1));
}
TEST(ShareTest, floatMulExtraLarge)
{
    Config *conf = Config::getInstance();
    int n_parties = conf->n_parties;
    std::vector<qmpc::Share::Share<float>> a(100000, 1);
    std::vector<qmpc::Share::Share<float>> b(100000, 1);
    a = a * b;
    open(a);
    auto rec = recons(a);
    EXPECT_EQ(rec[0], (n_parties * 1) * (n_parties * 1));
}
*/

TEST(ShareTest, FPMulExtraLarge)
{
    Config *conf = Config::getInstance();
    int n_parties = conf->n_parties;
    std::vector<qmpc::Share::Share<FixedPoint>> a(100000, FixedPoint("1"));
    std::vector<qmpc::Share::Share<FixedPoint>> b(100000, FixedPoint("1"));
    a = a * b;
    open(a);
    auto rec = recons(a);

    EXPECT_EQ(rec[0], (n_parties * 1) * (n_parties * 1));
}

TEST(ShareTest, unitvPreptest)
{
    auto [r, v] = qmpc::Share::unitvPrep<32>();
    open(v);
    open(r);
    auto rec_r = recons(r);
    auto rec_v = recons(v);
    rec_r = (rec_r % 32 + 32) % 32;
    std::cout << "r is " << rec_r << std::endl;
    EXPECT_EQ(rec_v[rec_r], 1);
}
TEST(ShareTest, unitvTest)
{
    qmpc::Share::Share<int> n(5);
    auto w = qmpc::Share::unitv(n);
    open(w);
    open(n);
    auto rec_w = recons(w);
    auto rec_n = recons(n);
    std::cout << "n is " << rec_n << std::endl;
    EXPECT_EQ(rec_w[rec_n % 32], 1);
}

TEST(ShareTest, expandTest)
{
    const std::array<int, 7> delta = {5, 5, 5, 5, 5, 5, 2};
    {
        int x = 5;  // 00000011
        auto d = qmpc::Share::expand(x, delta);
        std::vector<int> expected = {0, 0, 0, 0, 0, 0, 0, 5};
        for (size_t i = 0; i < d.size(); ++i)
        {
            EXPECT_EQ(expected[i], d[i]);
        }
    }
    {
        int y = -5;
        auto d = qmpc::Share::expand(y, delta);
        std::vector<int> expected = {0, 3, 31, 31, 31, 31, 31, 27};
        for (size_t i = 0; i < d.size(); ++i)
        {
            EXPECT_EQ(expected[i], d[i]);
        }
    }
}

TEST(ShareTest, equality1Test)
{
    qmpc::Share::Share<int> x(5);
    qmpc::Share::Share<int> y(5);
    qmpc::Share::Share<int> z(15);

    auto t = qmpc::Share::equality1(x, y);
    auto f = qmpc::Share::equality1(x, z);

    open(t);
    open(f);
    auto t_rec = recons(t);
    auto f_rec = recons(f);
    EXPECT_EQ(t_rec, 1);
    EXPECT_EQ(f_rec, 0);
}

TEST(ShareTest, equalityTest)
{
    qmpc::Share::Share<int> x(1 << 31);
    qmpc::Share::Share<int> y(1 << 31);
    qmpc::Share::Share<int> z(15);
    int minus = std::numeric_limits<int>::min();
    qmpc::Share::Share<int> minusx(minus);
    qmpc::Share::Share<int> minusy(minus);
    auto t = qmpc::Share::equality(x, y);
    auto f = qmpc::Share::equality(x, z);

    {
        const auto clock_start = std::chrono::system_clock::now();
        auto b = minusx == minusy;
        open(b);
        auto b_rec = recons(b);
        const auto clock_end = std::chrono::system_clock::now();
        const auto elapsed_time_ms =
            std::chrono::duration_cast<std::chrono::milliseconds>(clock_end - clock_start).count();
        QMPC_LOG_INFO("[{0}] Elapsed time = {1} ms", "eq", elapsed_time_ms);

        EXPECT_EQ(b_rec, 1);
    }

    {
        const auto clock_start = std::chrono::system_clock::now();
        auto b = x == y;
        open(b);
        auto b_rec = recons(b);
        const auto clock_end = std::chrono::system_clock::now();
        const auto elapsed_time_ms =
            std::chrono::duration_cast<std::chrono::milliseconds>(clock_end - clock_start).count();
        QMPC_LOG_INFO("[{0}] Elapsed time = {1} ms", "new eq", elapsed_time_ms);

        EXPECT_EQ(b_rec, 1);
    }
    open(t);
    open(f);
    auto t_rec = recons(t);
    auto f_rec = recons(f);
    EXPECT_EQ(t_rec, 1);
    EXPECT_EQ(f_rec, 0);
}
