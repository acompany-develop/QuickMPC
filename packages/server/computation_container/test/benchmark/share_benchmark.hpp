#pragma once
#include <algorithm>
#include <iostream>
#include <random>
#include <string>
#include <thread>

#include "benchmark.hpp"
#include "gtest/gtest.h"
#include "random/random.hpp"
#include "server/computation_to_computation_container/server.hpp"
#include "share/compare.hpp"
#include "share/share.hpp"
#include "unistd.h"

TEST(ShareBenchmark, AddBetweenShares)
{
    Share a(FixedPoint("1.0"));
    Share b(FixedPoint("2.0"));

    measureExecTime("AddBetweenShares", 5, [&]() { auto c = a + b; });
}

TEST(ShareBenchmark, SubBetweenShares)
{
    Share a(FixedPoint("2.0"));
    Share b(FixedPoint("1.0"));

    measureExecTime("SubBetweenShares", 5, [&]() { auto c = a - b; });
}

TEST(ShareBenchmark, MulBetweenShares)
{
    Share a(FixedPoint("3.0"));
    Share b(FixedPoint("3.0"));

    measureExecTime("MulBetweenShares", 5, [&]() { auto c = a * b; });
}

TEST(ShareBenchmark, DivBetweenShares)
{
    Share a(FixedPoint("1.0"));
    Share b(FixedPoint("2.0"));

    measureExecTime("DivBetweenShares", 5, [&]() { auto c = a / b; });
}

TEST(ShareBenchmark, AddBetweenShareAndFixedPoint)
{
    Share a(FixedPoint("1.0"));
    FixedPoint b("2.0");

    measureExecTime("AddBetweenShareAndFixedPoint", 5, [&]() { auto c = a + b; });
}

TEST(ShareBenchmark, SubBetweenShareAndFixedPoint)
{
    Share a(FixedPoint("10.0"));
    FixedPoint b("2.0");

    measureExecTime("SubBetweenShareAndFixedPoint", 5, [&]() { auto c = a - b; });
}

TEST(ShareBenchmark, MulBetweenShareAndFixedPoint)
{
    Share a(FixedPoint("2.0"));
    FixedPoint b("3.0");

    measureExecTime("MulBetweenShareAndFixedPoint", 5, [&]() { auto c = a * b; });
}

TEST(ShareBenchmark, DivBetweenShareAndFixedPoint)
{
    Share a(FixedPoint("1.0"));
    FixedPoint b("2.0");

    measureExecTime("DivBetweenShareAndFixedPoint", 5, [&]() { auto c = a / b; });
}

// 各要素の加法に関する逆元を一括で求めるテスト
TEST(ShareBenchmark, GetAdditiveInvVec)
{
    std::vector<Share> a = {
        Share(FixedPoint("5")),
        Share(FixedPoint("3.6")),
        Share(FixedPoint("-6")),
        Share(FixedPoint("-4.2")),
        Share(FixedPoint("0"))};

    measureExecTime("GetAdditiveInvVec", 5, [&]() { auto b = getAdditiveInvVec(a); });
}

// 各要素の乗法に関する逆元を一括で求めるテスト
TEST(ShareBenchmark, GetMultiplicativeInvVec)
{
    std::vector<Share> a = {
        Share(FixedPoint("5")),
        Share(FixedPoint("3.6")),
        Share(FixedPoint("-6")),
        Share(FixedPoint("-4.2"))};

    measureExecTime("GetMultiplicativeInvVec", 5, [&]() { auto b = getMultiplicativeInvVec(a); });
}

TEST(ShareBenchmark, AddBetweenSharesAndFixedPoint)
{
    std::vector<Share> a = {
        Share(FixedPoint("5")),
        Share(FixedPoint("3.6")),
        Share(FixedPoint("-6")),
        Share(FixedPoint("-4.2"))};
    FixedPoint b("2.0");

    // IntagrationTest/share_test.hpp からコピペ
    // share_test では a + b と b + a をやっているが，ここでは a + b だけ
    measureExecTime("AddBetweenSharesAndFixedPoint", 5, [&]() { auto c = a + b; });
}

TEST(ShareBenchmark, SubBetweenSharesAndFixedPoint)
{
    std::vector<Share> a = {
        Share(FixedPoint("5")),
        Share(FixedPoint("3.6")),
        Share(FixedPoint("-6")),
        Share(FixedPoint("-4.2"))};
    FixedPoint b("2.0");

    // IntagrationTest/share_test.hpp からコピペ
    // share_test では a - b と b - a をやっているが，ここでは a - b だけ
    measureExecTime("SubBetweenSharesAndFixedPoint", 5, [&]() { auto c = a - b; });
}

TEST(ShareBenchmark, MulBetweenSharesAndFixedPoint)
{
    std::vector<Share> a = {
        Share(FixedPoint("5")),
        Share(FixedPoint("3.6")),
        Share(FixedPoint("-6")),
        Share(FixedPoint("-4.2"))};
    FixedPoint b("2.0");

    // IntagrationTest/share_test.hpp からコピペ
    // share_test では a * b と b * a をやっているが，ここでは a * b だけ
    measureExecTime("MulBetweenSharesAndFixedPoint", 5, [&]() { auto c = a * b; });
}

TEST(ShareBenchmark, DivBetweenSharesAndFixedPoint)
{
    std::vector<Share> a = {
        Share(FixedPoint("5")),
        Share(FixedPoint("3.6")),
        Share(FixedPoint("-6")),
        Share(FixedPoint("-4.2"))};
    FixedPoint b("2.0");

    // IntagrationTest/share_test.hpp からコピペ
    // share_test では a / b と b / a をやっているが，ここでは a / b だけ
    measureExecTime("DivBetweenSharesAndFixedPoint", 5, [&]() { auto c = a / b; });
}

TEST(ShareBenchmark, ModuloBetweenShareAndFixedPoint)
{
    // IntagrationTest/share_test.hpp からコピペ
    // share_test にある テスト14 のみを計測
    Share a = Share(FixedPoint("-430333161.0"));
    FixedPoint b = FixedPoint("683330992.0");
    measureExecTime("ModuloBetweenShareAndFixedPoint", 5, [&]() { auto c = a % b; });
}

TEST(ShareBenchmark, AddBetweenFixedPointAndShare)
{
    Share a(FixedPoint("1.0"));
    FixedPoint b("2.0");

    measureExecTime("AddBetweenFixedPointAndShare", 5, [&]() { auto c = b + a; });
}

TEST(ShareBenchmark, SubBetweenFixedPointAndShare)
{
    Share a(FixedPoint("2.0"));
    FixedPoint b("10.0");

    measureExecTime("SubBetweenFixedPointAndShare", 5, [&]() { auto c = b - a; });
}

TEST(ShareBenchmark, MulBetweenFixedPointAndShare)
{
    Share a(FixedPoint("2.0"));
    FixedPoint b("3.0");

    measureExecTime("MulBetweenFixedPointAndShare", 5, [&]() { auto c = b * a; });
}

TEST(ShareBenchmark, DivBetweenFixedPointAndShare)
{
    Share a(FixedPoint("2.0"));
    FixedPoint b("12.0");

    measureExecTime("DivBetweenFixedPointAndShare", 5, [&]() { auto c = b / a; });
}

TEST(ShareBenchmark, RandBitShare)
{
    // ランダム要素を含むため，計測回数は1回
    measureExecTime(
        "RandBitShare", 1, [&]() { auto a = qmpc::Share::getRandBitShare<FixedPoint>(); }
    );
}

// 一括RandBitShareのテスト
TEST(ShareBenchmark, BulkRandBitShare)
{
    constexpr auto SIZE = 5;

    const auto test_name = getTestNameWithSize("BulkRandBitShare", SIZE);
    // ランダム要素を含むため，計測回数は1回
    measureExecTime(
        test_name, 1, [&]() { auto a = qmpc::Share::getRandBitShare<FixedPoint>(SIZE); }
    );
}

TEST(ShareBenchmark, LSBShare)
{
    // IntagrationTest/share_test.hpp からコピペ
    // ShareTestから1個取ってきた
    Share s = Share(FixedPoint("5"));

    // ランダム要素を含むため，計測回数は1回
    measureExecTime("LSBShare", 1, [&]() { auto lsb = qmpc::Share::getLSBShare(s); });
}

// 一括LSBShareのテスト
TEST(ShareBenchmark, BulkLSBShare)
{
    // IntagrationTest/share_test.hpp からコピペ
    // ShareTestと同じ
    std::vector<Share> s = {
        Share(FixedPoint("5")),
        Share(FixedPoint("6")),
        Share(FixedPoint("7.1")),
        Share(FixedPoint("7.4"))};

    const auto test_name = getTestNameWithSize("BulkLSbShare", s.size());
    // ランダム要素を含むため，計測回数は1回
    measureExecTime(test_name, 1, [&]() { auto lsb = qmpc::Share::getLSBShare(s); });
}

TEST(ShareBenchmark, Floor)
{
    // IntagrationTest/share_test.hpp からコピペ
    // ShareTestから1個取ってきた
    Share s = Share(FixedPoint("3.5"));

    // ランダム要素を含むため，計測回数は1回
    measureExecTime("Floor", 1, [&]() { auto s_floor = qmpc::Share::getFloor(s); });
}

// 一括Floorのテスト
TEST(ShareBenchmark, BulkFloor)
{
    // IntagrationTest/share_test.hpp からコピペ
    // ShareTestと同じ
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

    const auto test_name = getTestNameWithSize("BulkFloor", s.size());
    // ランダム要素を含むため，計測回数は1回
    measureExecTime(test_name, 1, [&]() { auto s_floor = qmpc::Share::getFloor(s); });
}

// LTZ (= Less Than Zero)
TEST(ShareBenchmark, LTZ)
{
    // IntagrationTest/share_test.hpp からコピペ
    // ShareTestから1個取ってきた
    Share s = Share(FixedPoint("-3.0"));
    // ランダム要素を含むため，計測回数は1回
    measureExecTime("LTZ", 1, [&]() { Share s_ltz = qmpc::Share::LTZ(s); });
}

// Share(とFixedPoint)での比較が可能かテストする
TEST(ShareBenchmark, ComparisonOperation)
{
    Share a(FixedPoint("2.0"));
    Share b(FixedPoint("3.0"));
    Share c(FixedPoint("3.0"));
    FixedPoint d("9.0");
    constexpr int N = 5;

    measureExecTime(
        "ComparisonOperation.LtBetweenShares", N, [&]() { [[maybe_unused]] auto e = a < b; }
    );
    measureExecTime(
        "ComparisonOperation.GtBetweenShares", N, [&]() { [[maybe_unused]] auto e = a > b; }
    );
    measureExecTime(
        "ComparisonOperation.LeqBetweenShares", N, [&]() { [[maybe_unused]] auto e = a <= b; }
    );
    measureExecTime(
        "ComparisonOperation.GeqBetweenShares", N, [&]() { [[maybe_unused]] auto e = a >= b; }
    );
    measureExecTime(
        "ComparisonOperation.NeqBetweenShares", N, [&]() { [[maybe_unused]] auto e = a != b; }
    );
    measureExecTime(
        "ComparisonOperation.EqBetweenShares", N, [&]() { [[maybe_unused]] auto e = b == c; }
    );

    // 交換しても時間は変わらないので Share <op> FixedPoint のみ
    measureExecTime(
        "ComparisonOperation.LtBetweenShareAndFixedPoint",
        N,
        [&]() { [[maybe_unused]] auto e = a < d; }
    );
    measureExecTime(
        "ComparisonOperation.GtBetweenShareAndFixedPoint",
        N,
        [&]() { [[maybe_unused]] auto e = a > d; }
    );
    measureExecTime(
        "ComparisonOperation.LeqBetweenShareAndFixedPoint",
        N,
        [&]() { [[maybe_unused]] auto e = a <= d; }
    );
    measureExecTime(
        "ComparisonOperation.GeqBetweenShareAndFixedPoint",
        N,
        [&]() { [[maybe_unused]] auto e = a >= d; }
    );
    measureExecTime(
        "ComparisonOperation.NeqBetweenShareAndFixedPoint",
        N,
        [&]() { [[maybe_unused]] auto e = c != d; }
    );
    measureExecTime(
        "ComparisonOperation.EqBetweenShareAndFixedPoint",
        N,
        [&]() { [[maybe_unused]] auto e = c == d; }
    );
}

TEST(ShareBenchmark, BulkComparisonOperation)
{
    std::vector<int> bench_size{1, 10, 100, 1000};
    for (const auto& size : bench_size)
    {
        std::vector<Share> l(size);
        std::vector<Share> r(size);
        const auto test_name_bulk = getTestNameWithSize("BulkComparisonOperation.BulkLess", size);
        measureExecTime(test_name_bulk, 1, [&]() { allLess(l, r); });
    }
    for (const auto& size : bench_size)
    {
        std::vector<Share> l(size);
        std::vector<Share> r(size);
        const auto test_name_each = getTestNameWithSize("BulkComparisonOperation.EachLess", size);
        measureExecTime(
            test_name_each,
            1,
            [&]()
            {
                for (int i = 0; i < size; ++i)
                {
                    l[i] < r[i];
                }
            }
        );
    }
}

// 一括open,reconsテスト
TEST(ShareBenchmark, ReconsBulk)
{
    constexpr int SIZE = 10000;
    std::vector<Share> t;
    for (auto i = 1; i <= SIZE; ++i)
    {
        t.emplace_back(FixedPoint(i));
    }

    std::vector<Share> s;
    constexpr auto N = 5;
    const auto test_name_open = getTestNameWithSize("ReconsBulk.Open", SIZE);
    measureExecTime(
        test_name_open,
        N,
        [&]()
        {
            s = t;
            open(s);
        }
    );

    const auto test_name_recons = getTestNameWithSize("ReconsBulk.recons", SIZE);
    // TODO: recons の計測回数を N 回にする
    // 現在，recons を複数回呼び出すとデッドロックが発生するため，
    // 計測回数を1回にしている
    measureExecTime(test_name_recons, 1, [&]() { auto u = recons(s); });
}

// 一括加算のテスト
TEST(ShareBenchmark, AddBulk)
{
    constexpr int SIZE = 10000;
    std::vector<Share> a, b;
    for (auto i = 1; i <= SIZE; ++i)
    {
        a.emplace_back(FixedPoint(i));
        b.emplace_back(FixedPoint(i));
    }

    const auto test_name = getTestNameWithSize("AddBulk", SIZE);
    constexpr auto N = 5;
    measureExecTime(test_name, N, [&]() { auto ret = a + b; });
}

// 一括減算のテスト
TEST(ShareBenchmark, SubBulk)
{
    constexpr int SIZE = 10000;
    std::vector<Share> a, b;
    for (auto i = 1; i <= SIZE; ++i)
    {
        a.emplace_back(FixedPoint(i));
        b.emplace_back(FixedPoint(SIZE - i));
    }

    const auto test_name = getTestNameWithSize("SubBulk", SIZE);
    constexpr auto N = 5;
    measureExecTime(test_name, N, [&]() { auto ret = a - b; });
}

// 一括乗算のテスト
TEST(ShareBenchmark, MulBulk)
{
    constexpr int SIZE = 10000;
    std::vector<Share> a, b;
    for (auto i = 1; i <= SIZE; ++i)
    {
        a.emplace_back(FixedPoint(i));
        b.emplace_back(FixedPoint(i));
    }

    const auto test_name = getTestNameWithSize("MulBulk", SIZE);
    constexpr auto N = 5;
    measureExecTime(test_name, N, [&]() { auto ret = a * b; });
}

TEST(ShareBenchmark, Sort)
{
    constexpr int MIN = 0;
    constexpr int MAX = 1;
    constexpr int SIZE = 5;

    std::vector<Share> x(SIZE);

    std::random_device rd;
    std::default_random_engine eng(rd());
    std::uniform_real_distribution<double> distr(MIN, MAX);

    for (int i = 0; i < SIZE; ++i)
    {
        x[i] = FixedPoint(distr(eng));
    }
    auto y = x;

    constexpr auto N = 5;
    measureExecTime("Sort.Share", N, [&]() { std::sort(x.begin(), x.end()); });

    open(y);
    auto y_rec = recons(y);

    measureExecTime("Sort.Svalue", N, [&]() { std::sort(y_rec.begin(), y_rec.end()); });
}

// sqrtのテスト
TEST(ShareBenchmark, Sqrt)
{
    // IntagrationTest/share_test.hpp からコピペ
    // share_test にある b = 121 のみ計測
    Share b;
    b += FixedPoint(121);

    measureExecTime("Sqrt", 5, [&]() { auto b_sqrt = qmpc::Share::sqrt(b); });
}

TEST(ShareBench, unarySend)
{
    for (int i = 0; i < 1000; ++i)
    {
        Share a = Share(FixedPoint("2.0"));
        open(a);
        auto rec = recons(a);
    }
    measureExecTime(
        "unaryMulti",
        4,
        [&]()
        {
            for (int i = 0; i < 1000; ++i)
            {
                Share a = Share(FixedPoint("2.0"));
                open(a);
                auto rec = recons(a);
            }
        }
    );
}

TEST(ShareBench, vecOpen)
{
    int N = 20000;
    {
        std::vector<Share> a;

        for (int i = 0; i < N; ++i)
        {
            a.emplace_back(FixedPoint("3"));
        }
        open(a);
        auto a_rec = recons(a);
    }

    measureExecTime(
        "vectorMul",
        4,
        [&]()
        {
            std::vector<Share> a;

            std::vector<Share> b;
            for (int i = 0; i < N; ++i)
            {
                a.emplace_back(FixedPoint("3"));  // 9
                b.emplace_back(FixedPoint("4"));  // 12
            }
            auto c = a * b;  // 108
            open(c);
            auto c_rec = recons(c);
        }
    );
}