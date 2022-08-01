#pragma once
#include "gtest/gtest.h"
#include <iostream>
#include <string>
#include <thread>
#include "unistd.h"
#include "Share/Share.hpp"
#include "Share/Compare.hpp"
#include "Random/Random.hpp"
#include "Server/ComputationToComputationContainer/Server.hpp"
#include <algorithm>
#include <random>

#include "LogHeader/Logger.hpp"

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
    spdlog::info("a_rec = {}", a_rec.getDoubleVal());
    EXPECT_EQ(a_rec, FixedPoint(std::to_string((3.0 * n_parties) * (3.0 * n_parties))));
}

TEST(ShareTest, DivBetweenShares)
{
    Share a(FixedPoint("1.0"));
    Share b(FixedPoint("2.0"));
    a = a / b;
    open(a);
    FixedPoint a_rec = recons(a);
    spdlog::info("a_rec = {}", a_rec.getDoubleVal());
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
    spdlog::info("ans = {}", ans.getStrVal());
    EXPECT_EQ(ans, FixedPoint(std::to_string(6 * n_parties % 7)));

    // テスト2
    a = Share(FixedPoint("-15.0"));
    b = FixedPoint("11.0");
    ans = a % b;
    spdlog::info("ans = {}", ans.getStrVal());
    EXPECT_EQ(ans, FixedPoint(std::to_string(((-15 * n_parties % 11) + 11) % 11)));

    // テスト3
    a = Share(FixedPoint("17.0"));
    b = FixedPoint("-4.0");
    ans = a % b;
    spdlog::info("ans = {}", ans.getStrVal());
    EXPECT_EQ(ans, FixedPoint(std::to_string(((17 * n_parties % -4) - 4) % -4)));

    // テスト4
    a = Share(FixedPoint("-13.0"));
    b = FixedPoint("-6.0");
    ans = a % b;
    spdlog::info("ans = {}", ans.getStrVal());
    EXPECT_EQ(ans, FixedPoint(std::to_string(-13 * n_parties % -6)));

    // テスト5
    a = Share(FixedPoint("10453.0"));
    b = FixedPoint("129.0");
    ans = a % b;
    spdlog::info("ans = {}", ans.getStrVal());
    EXPECT_EQ(ans, FixedPoint(std::to_string(10453 * n_parties % 129)));

    // テスト6
    a = Share(FixedPoint("-33914.0"));
    b = FixedPoint("529.0");
    ans = a % b;
    spdlog::info("ans = {}", ans.getStrVal());
    EXPECT_EQ(ans, FixedPoint(std::to_string(((-33914 * n_parties % 529) + 529) % 529)));

    // テスト7
    a = Share(FixedPoint("20991.0"));
    b = FixedPoint("-1034.0");
    ans = a % b;
    spdlog::info("ans = {}", ans.getStrVal());
    EXPECT_EQ(ans, FixedPoint(std::to_string(((20991 * n_parties % -1034) - 1034) % -1034)));

    // テスト8
    a = Share(FixedPoint("-18001.0"));
    b = FixedPoint("-4034.0");
    ans = a % b;
    spdlog::info("ans = {}", ans.getStrVal());
    EXPECT_EQ(ans, FixedPoint(std::to_string(-18001 * n_parties % -4034)));

    // テスト9
    a = Share(FixedPoint("7535565.0"));
    b = FixedPoint("9617262.0");
    ans = a % b;
    spdlog::info("ans = {}", ans.getStrVal());
    EXPECT_EQ(ans, FixedPoint(std::to_string(7535565 * n_parties % 9617262)));

    // テスト10
    a = Share(FixedPoint("-1190027.0"));
    b = FixedPoint("3492693.0");
    ans = a % b;
    spdlog::info("ans = {}", ans.getStrVal());
    EXPECT_EQ(
        ans, FixedPoint(std::to_string(((-1190027 * n_parties % 3492693) + 3492693) % 3492693))
    );

    // テスト11
    a = Share(FixedPoint("2957085.0"));
    b = FixedPoint("-15625920.0");
    ans = a % b;
    spdlog::info("ans = {}", ans.getStrVal());
    EXPECT_EQ(
        ans, FixedPoint(std::to_string(((2957085 * n_parties % -15625920) - 15625920) % -15625920))
    );

    // テスト12
    a = Share(FixedPoint("-11866897.0"));
    b = FixedPoint("-15090045.0");
    ans = a % b;
    spdlog::info("ans = {}", ans.getStrVal());
    EXPECT_EQ(ans, FixedPoint(std::to_string(-11866897 * n_parties % -15090045)));

    // テスト13
    a = Share(FixedPoint("478935003.0"));
    b = FixedPoint("315492017.0");
    ans = a % b;
    spdlog::info("ans = {}", ans.getStrVal());
    EXPECT_EQ(ans, FixedPoint(std::to_string(478935003 * n_parties % 315492017)));

    // テスト14
    a = Share(FixedPoint("-430333161.0"));
    b = FixedPoint("683330992.0");
    ans = a % b;
    spdlog::info("ans = {}", ans.getStrVal());
    EXPECT_EQ(
        ans,
        FixedPoint(std::to_string(((-430333161 * n_parties % 683330992) + 683330992) % 683330992))
    );

    // テスト15
    a = Share(FixedPoint("784585987.0"));
    b = FixedPoint("-311360387.0");
    ans = a % b;
    spdlog::info("ans = {}", ans.getStrVal());
    EXPECT_EQ(
        ans,
        FixedPoint(std::to_string(((784585987LL * n_parties % -311360387) - 311360387) % -311360387)
        )
    );

    // テスト16
    a = Share(FixedPoint("-427544319.0"));
    b = FixedPoint("-818477560.0");
    ans = a % b;
    spdlog::info("ans = {}", ans.getStrVal());
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
    spdlog::info("a_rec = {}", a_rec.getDoubleVal());
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
        spdlog::info("RandBit = {}", a_rec.getDoubleVal());
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
        spdlog::info("RandBit = {}", a_rec[i].getDoubleVal());
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
        spdlog::info("LSB({}) = {}", expected[i][0], lsb_rec.getDoubleVal());
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
        spdlog::info("LSB({}) = {}", expected[i][0], lsb_rec[i].getDoubleVal());
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
        spdlog::info("floor({}) = {}", expected[i][0], result);
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
        spdlog::info("floor({}) = {}", expected[i][0], result[i]);
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
        spdlog::info("[{}<0] {}", expected[i][0], result);
        EXPECT_NEAR(result.getDoubleVal(), expected[i][1], error);
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

// ランダムな値で比較演算のテストを実行
TEST(ShareTest, RandomComparisonOperation)
{
    {
        // Share と Share の比較
        Share a_share(RandGenerator::getInstance()->getRand<FixedPoint>(-10000, 10000));
        open(a_share);
        Share b_share(RandGenerator::getInstance()->getRand<FixedPoint>(-10000, 10000));
        open(b_share);
        spdlog::info("a_share is {}", a_share.getVal());
        spdlog::info("b_share is {}", b_share.getVal());
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
    spdlog::info("one step !!");
    // Share と FixedPoint の比較
    {
        Share a_share(RandGenerator::getInstance()->getRand<FixedPoint>(1, 1000));
        open(a_share);
        FixedPoint a_rec = recons(a_share);
        FixedPoint target = FixedPoint("0");
        spdlog::info("a_rec = \t{}", a_rec.getVal());
        spdlog::info("target = \t{}", target.getVal());
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
    spdlog::info("two step !!");
    // FixedPoint と Share の比較
    {
        Share a_share(RandGenerator::getInstance()->getRand<FixedPoint>(1, 1000));
        open(a_share);
        FixedPoint a_rec = recons(a_share);
        FixedPoint target = FixedPoint("0");
        spdlog::info("a_rec = \t{}", a_rec.getVal());
        spdlog::info("target = \t{}", target.getVal());
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

//定数値Shareテスト
TEST(ShareTest, ConstantShare)
{
    auto fp = FixedPoint("12");
    auto s = qmpc::Share::getConstantShare(fp);
    open(s);
    auto fp_r = recons(s);
    EXPECT_EQ(fp, fp_r);
}

//一括open,reconsテスト
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

//一括加算のテスト
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

    spdlog::info("AddBulk End !!");
    EXPECT_EQ(target, expected);
}

//一括減算のテスト
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

    spdlog::info("SubBulk End !!");
    EXPECT_EQ(target, expected);
}

//一括乗算のテスト
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

    spdlog::info("MulBulk End !!");
    for (int i = 0; i < static_cast<int>(a.size()); ++i)
    {
        spdlog::info(target[i].getStrVal());
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
            spdlog::info("Firset thread is {}", ret1.getVal());
            spdlog::info("First ID is {}", a.getId());
        }
    );
    std::thread th2(
        [&]()
        {
            qmpc::Share::AddressId::setJobId(1);
            Share a(0);
            open(a);
            ret2 = recons(a);
            spdlog::info("Second thread is {}", ret2.getVal());
            spdlog::info("Second ID is {}", a.getId());
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

    spdlog::info("share sorting time is : {}", dsec);
    open(y);
    auto y_rec = recons(y);

    start = std::chrono::system_clock::now();
    std::sort(y_rec.begin(), y_rec.end());

    end = std::chrono::system_clock::now();
    diff = end - start;
    dsec = std::chrono::duration_cast<std::chrono::milliseconds>(diff).count();

    spdlog::info("svalue sorting time is : {}", dsec);

    // for (auto &&aa : x)
    // {
    //     spdlog::info("{}",aa.getVal());
    // }
    open(x);
    auto x_rec = recons(x);
    for (int i = 0; i < N; ++i)
    {
        // spdlog::info("X is {}",x_rec[i]);
        // spdlog::info("Y is {}", y_rec[i]);
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

    spdlog::info("3 * n_parties sqrt is {}", target);
    EXPECT_NEAR(target.getDoubleVal(), std::sqrt(3 * n_parties), 0.01);

    Share b;
    b += FixedPoint(121);
    auto b_sqrt = qmpc::Share::sqrt(b);
    open(b_sqrt);
    auto targetb = recons(b_sqrt);

    spdlog::info("121 sqrt is {}", targetb);
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
    bool ng = false;

    for (int i = 0; i < static_cast<int>(t.size()); ++i)
    {
        EXPECT_NEAR(target[i].getDoubleVal(), expected[i].getDoubleVal(), 0.00001);
    }
}
