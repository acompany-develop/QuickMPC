/*
 * コメントで明記していない限り，四則演算以外はランダム要素を含むため，
 * 計測回数を1回にしている．
 */

#pragma once
#include <iostream>
#include <string>
#include <thread>

#include "Benchmark.hpp"
#include "Server/ComputationToComputationContainer/Server.hpp"
#include "Share/Compare.hpp"
#include "Share/Share.hpp"
#include "gtest/gtest.h"
#include "unistd.h"

TEST(ShareCompBenchmark, AddBetweenShareComps)
{
    ShareComp a(PrimeField(1));
    ShareComp b(PrimeField(2));
    measureExecTime("AddBetweenShareComps", 5, [&]() { auto c = a + b; });
}

TEST(ShareCompBenchmark, SubBetweenShareComps)
{
    ShareComp a(PrimeField(2));
    ShareComp b(PrimeField(1));
    measureExecTime("SubBetweenShareComps", 5, [&]() { auto c = a - b; });
}

TEST(ShareCompBenchmark, MulBetweenShareComps)
{
    ShareComp a(PrimeField(3));
    ShareComp b(PrimeField(3));
    measureExecTime("MulBetweenShareComps", 5, [&]() { auto c = a * b; });
}

TEST(ShareCompBenchmark, DivBetweenShareComps)
{
    ShareComp a(PrimeField(1));
    ShareComp b(PrimeField(2));
    measureExecTime("DivBetweenShareComps", 5, [&]() { auto c = a / b; });
}

TEST(ShareCompBenchmark, AddBetweenShareCompAndPrimeField)
{
    ShareComp a(PrimeField(1));
    PrimeField b(2);
    // 交換しても時間は変わらないので a + b だけ計測
    measureExecTime("AddBetweenShareCompAndPrimeField", 5, [&]() { auto c = a + b; });
}

TEST(ShareCompBenchmark, SubBetweenShareCompAndPrimeField)
{
    ShareComp a(PrimeField(10));
    PrimeField b(2);
    // 交換しても時間は変わらないので a - b だけ計測
    measureExecTime("SubBetweenShareCompAndPrimeField", 5, [&]() { auto c = a - b; });
}

TEST(ShareCompBenchmark, MulBetweenShareCompAndPrimeField)
{
    ShareComp a(PrimeField(2));
    PrimeField b(3);
    // 交換しても時間は変わらないので a * b だけ計測
    measureExecTime("MulBetweenShareCompAndPrimeField", 5, [&]() { auto c = a * b; });
}

TEST(ShareCompBenchmark, DivBetweenShareCompAndPrimeField)
{
    ShareComp a(PrimeField(1));
    PrimeField b(2);
    // 交換しても時間は変わらないので a / b だけ計測
    measureExecTime("DivBetweenShareCompAndPrimeField", 5, [&]() { auto c = a / b; });
}

// 以下、サブプロトコルに関するテスト

// 0以上p未満の値であることを確認
TEST(ShareCompBenchmark, SubRNS)
{
    measureExecTime("SubRNS", 1, []() { auto a = qmpc::Share::subRNS(); });
}

// 復元した値が必ず0か1であることを確認
TEST(ShareCompBenchmark, SubRBS)
{
    measureExecTime("SubRBS", 1, []() { auto a = qmpc::Share::subRBS(); });
}

// 全てのビットシェアを復元したときに、その結果が0か1であることを確認
TEST(ShareCompBenchmark, SubRBVS)
{
    measureExecTime("SubRBVS", 1, []() { auto a_shares = qmpc::Share::subRBVS(); });
}

// 多入力論理和のシェアの復元値が0もしくは1であることを確認
// 基本的には1が復元。
TEST(ShareCompBenchmark, SubUnboundedFanInOr)
{
    // Test1. ランダムなビットシェアでテスト
    measureExecTime(
        "SubUnboundedFanInOr.subRBVS",
        1,
        []()
        {
            auto a_shares = qmpc::Share::subRBVS();
            auto a = qmpc::Share::subUnboundedFanInOr(a_shares);
        }
    );

    // Test2. 全ての復元ビットが0となるビットシェアでテスト
    measureExecTime(
        "SubUnboundedFanInOr.AllZeros",
        1,
        []()
        {
            auto b_shares = qmpc::Share::PFtoBS(PrimeField(0));
            ShareComp b = qmpc::Share::subUnboundedFanInOr(b_shares);
        }
    );
}

// 多入力論理積のシェアの復元値が0もしくは1であることを確認
// 基本的には0が復元。
TEST(ShareCompBenchmark, SubUnboundedFanInAnd)
{
    // Test1. ランダムなビットシェアでテスト
    measureExecTime(
        "SubUnboundedFanInAnd.subRBVS",
        1,
        []()
        {
            auto a_shares = qmpc::Share::subRBVS();
            auto a = qmpc::Share::subUnboundedFanInAnd(a_shares);
        }
    );

    // Test2. 全ての復元ビットが1となるビットシェアでテスト
    // TestSubUnboundedFanInOrの場合のように，PFtoBSで全ての復元ビットが1となる
    // ビットシェアを生成できないので、PFtoBS → BStoSC
    // の流れでビットシェアを生成．
    measureExecTime(
        "SubUnboundedFanInAnd.AllOnes",
        1,
        []()
        {
            std::vector<ShareComp> b_shares(PrimeField::l);
            auto tmp_shares = qmpc::Share::PFtoBS(PrimeField(1));
            auto b_share = qmpc::Share::BStoSC(tmp_shares);
            for (int i = 0; i < PrimeField::l; i++)
            {
                b_shares[i] = b_share;
            }
            auto b = qmpc::Share::subUnboundedFanInAnd(b_shares);
        }
    );
}

// 上位ビットから復元し，どのビットも0か1であることを確認
// さらに，最初に1が復元されたら，それ以降は必ず1が復元されることを確認
TEST(ShareCompBenchmark, SubPrefixOr)
{
    measureExecTime(
        "SubPrefixOr",
        1,
        []()
        {
            auto a_shares = qmpc::Share::subRBVS();
            auto ret = subPrefixOr(a_shares);
        }
    );
}

// a < bであるときに1, それ以外の場合に0であるかチェックする
TEST(ShareCompBenchmark, SubBitwiseLessThan)
{
    measureExecTime(
        "SubBitwiseLessThan",
        1,
        []()
        {
            auto a_shares = qmpc::Share::subRBVS();
            auto b_shares = qmpc::Share::subRBVS();
            ShareComp ret = subBitwiseLessThan(a_shares, b_shares);
        }
    );
}

// BitShareをShareCompに正しく変換できているかチェック
TEST(ShareCompBenchmark, BStoSC)
{
    measureExecTime(
        "BStoSC",
        1,
        []()
        {
            // IntegrationTest/ShareCompTest.hpp からコピペ
            // ShareCompTest のテストケースの中から1個取ってきた
            auto test_case = PrimeField(14055031461681146081ull);
            auto a_shares = qmpc::Share::PFtoBS(test_case);
            ShareComp ans = qmpc::Share::BStoSC(a_shares);
        }
    );
}

// PrimeFieldをBitshareに正しく変換できているかチェック
TEST(ShareCompBenchmark, PFtoBS)
{
    measureExecTime(
        "PFtoBS",
        1,
        []()
        {
            // IntegrationTest/ShareCompTest.hpp からコピペ
            // ShareCompTest のテストケースの中から1個取ってきた
            auto test_case = PrimeField(11990882371217920413ull);
            auto a_shares = qmpc::Share::PFtoBS(test_case);
        }
    );
}

// a < p/2の場合は1, それ以外の場合は0であるかチェックする
TEST(ShareCompBenchmark, SubHalfLessThan)
{
    measureExecTime(
        "SubHalfLessThan",
        1,
        []()
        {
            ShareComp a_share = qmpc::Share::subRNS();
            ShareComp ret = qmpc::Share::subHalfLessThan(a_share);
        }
    );
}

// 以下、メインプロトコルのテスト

// c1 < a < c2の場合は1, そうでない場合は0を返す
TEST(ShareCompBenchmark, IntervalTest)
{
    measureExecTime(
        "IntervalTest",
        1,
        []()
        {
            // IntegrationTest/ShareCompTest.hpp からコピペ
            // ShareCompTest から c1 < a < c2
            // が成立するテストケースのみを抜粋した
            PrimeField c1(9000000);
            PrimeField c2(10000000);
            std::vector<ShareComp> bit_shares = qmpc::Share::PFtoBS(PrimeField(9500000));
            ShareComp a_share = qmpc::Share::BStoSC(bit_shares);
            ShareComp interval_share = qmpc::Share::intervalTest(c1, a_share, c2);
        }
    );
}

// a < bの場合は1, そうでない場合は0を返す
TEST(ShareCompBenchmark, ComparisonTest)
{
    measureExecTime(
        "ComparisonTest",
        1,
        []()
        {
            // IntegrationTest/ShareCompTest.hpp からコピペ
            // ShareCompTest から a < b が成立するテストケースのみを抜粋した
            auto a_bit_shares = qmpc::Share::PFtoBS(PrimeField(9500000));
            auto b_bit_shares = qmpc::Share::PFtoBS(PrimeField(10500000));
            ShareComp a_share = qmpc::Share::BStoSC(a_bit_shares);
            ShareComp b_share = qmpc::Share::BStoSC(b_bit_shares);
            ShareComp comp_share = qmpc::Share::comparisonTest(a_share, b_share);
        }
    );
}

// a = bの場合は1、そうでない場合は0を返す
TEST(ShareCompBenchmark, EqualityTest)
{
    measureExecTime(
        "EqualityTest",
        1,
        []()
        {
            // IntegrationTest/ShareCompTest.hpp からコピペ
            // ShareCompTest から a = b が成立するテストケースのみを抜粋した
            auto a_bit_shares = qmpc::Share::PFtoBS(PrimeField(10500000));
            auto b_bit_shares = qmpc::Share::PFtoBS(PrimeField(10500000));
            ShareComp a_share = qmpc::Share::BStoSC(a_bit_shares);
            ShareComp b_share = qmpc::Share::BStoSC(b_bit_shares);
            ShareComp e_share = qmpc::Share::equalityTest(a_share, b_share);
        }
    );
}

//一括open,reconsテスト
TEST(ShareCompBenchmark, ShareCompReconsBulk)
{
    // Benchmark/ShareBenchmark.hpp の
    // 一括open, reconsテスト (ReconsBulk) と同様のテスト

    constexpr int SIZE = 10000;
    std::vector<ShareComp> t;
    for (auto i = 1; i <= SIZE; ++i)
    {
        t.emplace_back(PrimeField(i));
    }

    std::vector<ShareComp> s;
    constexpr auto N = 5;
    const auto test_name_open = getTestNameWithSize("ShareCompReconsBulk.Open", SIZE);
    measureExecTime(
        test_name_open,
        N,
        [&s, &t]()
        {
            s = t;
            open(s);
        }
    );

    const auto test_name_recons = getTestNameWithSize("ShareCompReconsBulk.Recons", SIZE);
    // TODO: recons の計測回数を N 回にする
    // 現在，recons を複数回呼び出すとデッドロックが発生するため，
    // 計測回数を1回にしている
    measureExecTime(test_name_recons, 1, [&s]() { auto u = recons(s); });
}

//一括乗算のテスト
TEST(ShareCompBenchmark, ShareCompMulBulk)
{
    // Benchmark/ShareBenchmark.hpp の
    // 一括乗算テスト (MulBulk) と同様のテスト

    constexpr int SIZE = 10000;
    std::vector<ShareComp> a, b;
    for (auto i = 1; i <= SIZE; ++i)
    {
        a.emplace_back(PrimeField(i));
        b.emplace_back(PrimeField(i));
    }

    const auto test_name = getTestNameWithSize("ShareCompMulBulk", SIZE);
    constexpr auto N = 5;
    measureExecTime(test_name, N, [&a, &b]() { auto ret = a * b; });
}
