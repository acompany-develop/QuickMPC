#pragma once
#include "gtest/gtest.h"
#include <iostream>
#include <string>
#include <thread>
#include "unistd.h"
#include "Share/Share.hpp"
#include "Share/Compare.hpp"
#include "Server/ComputationToComputationContainer/Server.hpp"

#include "LogHeader/Logger.hpp"

TEST(ShareCompTest, IncrementShareCompId)
{
    ShareComp a(PrimeField(3));
    ShareComp b(PrimeField(3));
    EXPECT_EQ(a.getId().getShareId(), 0);
    EXPECT_EQ(b.getId().getShareId(), 1);
}

TEST(ShareCompTest, GetValue)
{
    ShareComp a(PrimeField(3));
    EXPECT_EQ(a.getVal(), PrimeField(3));
}

TEST(ShareCompTest, AddBetweenShareComps)
{
    Config *conf = Config::getInstance();
    int n_parties = conf->n_parties;
    ShareComp a(PrimeField(1));
    ShareComp b(PrimeField(2));
    a = a + b;
    open(a);
    PrimeField a_rec = recons(a);
    EXPECT_EQ(a_rec, PrimeField(3 * n_parties));
}

TEST(ShareCompTest, SubBetweenShareComps)
{
    Config *conf = Config::getInstance();
    int n_parties = conf->n_parties;
    ShareComp a(PrimeField(2));
    ShareComp b(PrimeField(1));
    a = a - b;
    open(a);
    PrimeField a_rec = recons(a);
    EXPECT_EQ(a_rec, PrimeField(n_parties));
}

TEST(ShareCompTest, MulBetweenShareComps)
{
    Config *conf = Config::getInstance();
    int n_parties = conf->n_parties;
    ShareComp a(PrimeField(3));
    ShareComp b(PrimeField(3));
    a = a * b;
    open(a);
    PrimeField a_rec = recons(a);
    EXPECT_EQ(a_rec, PrimeField((3 * n_parties) * (3 * n_parties)));
}

TEST(ShareCompTest, DivBetweenShareComps)
{
    ShareComp a(PrimeField(1));
    ShareComp b(PrimeField(2));
    a = a / b;
    open(a);
    PrimeField a_rec = recons(a);
    EXPECT_EQ(a_rec, PrimeField(9223372036854775779));
}

TEST(ShareCompTest, AddBetweenShareCompAndPrimeField)
{
    Config *conf = Config::getInstance();
    int n_parties = conf->n_parties;
    ShareComp a(PrimeField(1));
    PrimeField b(2);
    a = a + b;
    open(a);
    PrimeField a_rec = recons(a);
    EXPECT_EQ(a_rec, PrimeField(n_parties + 2));
}

TEST(ShareCompTest, SubBetweenShareCompAndPrimeField)
{
    Config *conf = Config::getInstance();
    int n_parties = conf->n_parties;
    ShareComp a(PrimeField(10));
    PrimeField b(2);
    a = a - b;
    open(a);
    PrimeField a_rec = recons(a);
    EXPECT_EQ(a_rec, PrimeField(10 * n_parties - 2));
}

TEST(ShareCompTest, MulBetweenShareCompAndPrimeField)
{
    Config *conf = Config::getInstance();
    int n_parties = conf->n_parties;
    ShareComp a(PrimeField(2));
    PrimeField b(3);
    a = a * b;
    open(a);
    PrimeField a_rec = recons(a);
    EXPECT_EQ(a_rec, PrimeField(2 * n_parties * 3));
}

TEST(ShareCompTest, DivBetweenShareCompAndPrimeField)
{
    Config *conf = Config::getInstance();
    int n_parties = conf->n_parties;
    ShareComp a(PrimeField(1));
    PrimeField b(2);
    a = a / b;
    open(a);
    PrimeField a_rec = recons(a);
    EXPECT_EQ(a_rec, n_parties * b.getInv());
}

TEST(ShareCompTest, AddBetweenPrimeFieldAndShareComp)
{
    Config *conf = Config::getInstance();
    int n_parties = conf->n_parties;
    ShareComp a(PrimeField(1));
    PrimeField b(2);
    a = b + a;
    open(a);
    PrimeField a_rec = recons(a);
    EXPECT_EQ(a_rec, PrimeField(2 + n_parties));
}

TEST(ShareCompTest, SubBetweenPrimeFieldAndShareComp)
{
    Config *conf = Config::getInstance();
    int n_parties = conf->n_parties;
    ShareComp a(PrimeField(2));
    PrimeField b(10);
    a = b - a;
    open(a);
    PrimeField a_rec = recons(a);
    EXPECT_EQ(a_rec, PrimeField(10 - 2 * n_parties));
}

TEST(ShareCompTest, MulBetweenPrimeFieldAndShareComp)
{
    Config *conf = Config::getInstance();
    int n_parties = conf->n_parties;
    ShareComp a(PrimeField(2));
    PrimeField b(3);
    a = b * a;
    open(a);
    PrimeField a_rec = recons(a);
    EXPECT_EQ(a_rec, PrimeField(3 * 2 * n_parties));
}

TEST(ShareCompTest, DivBetweenPrimeFieldAndShareComp)
{
    Config *conf = Config::getInstance();
    int n_parties = conf->n_parties;
    ShareComp a(PrimeField(2));
    PrimeField b(1);
    a = b / a;
    open(a);
    PrimeField a_rec = recons(a);
    EXPECT_EQ(a_rec, PrimeField(2 * n_parties).getInv());
}

// 以下、サブプロトコルに関するテスト

// 0以上p未満の値であることを確認
TEST(ShareCompTest, SubRNS)
{
    // TODO: 乱数を加味したテスト
    ShareComp a = qmpc::Share::subRNS();
    open(a);
    PrimeField a_rec = recons(a);
    EXPECT_GE(a_rec.getVal(), 0);
    EXPECT_LT(a_rec.getVal(), PrimeField::p);
}

// 復元した値が必ず0か1であることを確認
TEST(ShareCompTest, SubRBS)
{
    ShareComp a = qmpc::Share::subRBS();
    open(a);
    PrimeField a_rec = recons(a);
    spdlog::info("a_rec = {}", a_rec.getVal());
    EXPECT_TRUE(a_rec.getVal() == 0 || a_rec.getVal() == 1)
        << "a_rec.getVal() = " << a_rec.getVal();
}

// 全てのビットシェアを復元したときに、その結果が0か1であることを確認
TEST(ShareCompTest, SubRBVS)
{
    auto a_shares = qmpc::Share::subRBVS();
    bool flag = true;
    for (int i = 0; i < PrimeField::l; i++)
    {
        open(a_shares[i]);
        PrimeField a_rec = recons(a_shares[i]);
        spdlog::info("a_rec= {}", a_rec.getVal());
        if ((a_rec.getVal() != 0) && (a_rec.getVal() != 1))
        {
            flag = false;
            break;
        }
    }

    EXPECT_TRUE(flag);
}

// 多入力論理和のシェアの復元値が0もしくは1であることを確認
// 基本的には1が復元。
TEST(ShareCompTest, SubUnboundedFanInOr)
{
    // Test1. ランダムなビットシェアでテスト
    auto a_shares = qmpc::Share::subRBVS();
    ShareComp a = qmpc::Share::subUnboundedFanInOr(a_shares);
    open(a);
    PrimeField a_rec = recons(a);
    spdlog::info("a_rec = {}", a_rec.getVal());
    EXPECT_TRUE(a_rec.getVal() == 0 || a_rec.getVal() == 1)
        << "a_rec.getVal() = " << a_rec.getVal();

    // Test2. 全ての復元ビットが0となるビットシェアでテスト
    auto b_shares = qmpc::Share::PFtoBS(PrimeField(0));
    ShareComp b = qmpc::Share::subUnboundedFanInOr(b_shares);
    open(b);
    PrimeField b_rec = recons(b);
    spdlog::info("b_rec = {}", b_rec.getVal());
    EXPECT_EQ(b_rec.getVal(), 0);
}

// 多入力論理積のシェアの復元値が0もしくは1であることを確認
// 基本的には0が復元。
TEST(ShareCompTest, SubUnboundedFanInAnd)
{
    // Test1. ランダムなビットシェアでテスト
    auto a_shares = qmpc::Share::subRBVS();
    ShareComp a = qmpc::Share::subUnboundedFanInAnd(a_shares);
    open(a);
    PrimeField a_rec = recons(a);
    spdlog::info("a_rec= {}", a_rec.getVal());
    EXPECT_TRUE(a_rec.getVal() == 0 || a_rec.getVal() == 1)
        << "a_rec.getVal() = " << a_rec.getVal();

    // Test2. 全ての復元ビットが1となるビットシェアでテスト
    // TestSubUnboundedFanInOrの場合のように，PFtoBSで全ての復元ビットが1となる
    // ビットシェアを生成できないので、PFtoBS → BStoSC
    // の流れでビットシェアを生成．
    std::vector<ShareComp> b_shares(PrimeField::l);
    auto tmp_shares = qmpc::Share::PFtoBS(PrimeField(1));
    ShareComp b_share = qmpc::Share::BStoSC(tmp_shares);
    for (int i = 0; i < PrimeField::l; i++)
    {
        b_shares[i] = b_share;
    }
    ShareComp b = qmpc::Share::subUnboundedFanInAnd(b_shares);
    open(b);
    PrimeField b_rec = recons(b);
    spdlog::info("b_rec = {}", b_rec.getVal());
    EXPECT_EQ(b_rec.getVal(), 1);
}

// 上位ビットから復元し，どのビットも0か1であることを確認
// さらに，最初に1が復元されたら，それ以降は必ず1が復元されることを確認
TEST(ShareCompTest, SubPrefixOr)
{
    auto a_shares = qmpc::Share::subRBVS();
    auto ret = subPrefixOr(a_shares);
    bool flag = true;          // テスト失敗の場合はfalseに
    bool isAppearOne = false;  // 1が登場したらtrueに
    for (int i = 0; i < PrimeField::l; i++)
    {
        open(ret[i]);
        PrimeField ret_rec = recons(ret[i]);
        if (ret_rec.getVal() == 0)
        {
            if (isAppearOne)
            {  // 既に1が出現していたらテスト失敗
                flag = false;
                break;
            }
        }
        else if (ret_rec.getVal() == 1)
        {
            isAppearOne = true;
        }
        else if ((ret_rec.getVal() != 0) && (ret_rec.getVal() != 1))  // 0,
                                                                      // 1以外の値が復元されたらテスト失敗
        {
            flag = false;
            break;
        }
    }

    EXPECT_TRUE(flag);
}

// a < bであるときに1, それ以外の場合に0であるかチェックする
TEST(ShareCompTest, SubBitwiseLessThan)
{
    // loop回テストを走らせる
    for (int loop = 0; loop < 10; loop++)
    {
        auto a_shares = qmpc::Share::subRBVS();
        auto b_shares = qmpc::Share::subRBVS();

        // 以下，a<bの判定
        bool is_b_larger_than_a = false;  // a<bならばtrue
        bool check = true;                // a_rec ≠ b_recになった場合，falseに変更
        for (int i = 0; i < PrimeField::l; i++)
        {
            open(a_shares[i]);
            PrimeField a_rec = recons(a_shares[i]);
            open(b_shares[i]);
            PrimeField b_rec = recons(b_shares[i]);
            if (check)
            {
                if (a_rec.getVal() < b_rec.getVal())
                {
                    check = false;
                    is_b_larger_than_a = true;
                }
                else if (a_rec.getVal() > b_rec.getVal())
                {
                    check = false;
                }
            }
        }

        // 以下，BitwiseLessThanの出力が適切であるかチェック
        ShareComp ret = subBitwiseLessThan(a_shares, b_shares);
        open(ret);
        PrimeField recons_ret = recons(ret);
        spdlog::info("recons_ret = {}", recons_ret.getVal());
        if (is_b_larger_than_a)
        {
            EXPECT_EQ(recons_ret.getVal(), 1);
        }
        else
        {
            EXPECT_EQ(recons_ret.getVal(), 0);
        }
    }
}

// BitShareをShareCompに正しく変換できているかチェック
TEST(ShareCompTest, BStoSC)
{
    bool flag = true;  // テストが失敗した場合falseになる
    // テストケース
    PrimeField test_case[10] = {
        PrimeField(8404635061939753279),
        PrimeField(14055031461681146081),
        PrimeField(4401882840069225473),
        PrimeField(955727980728323728),
        PrimeField(11939286497068664120),
        PrimeField(7128445707545015008),
        PrimeField(13165665109915604075),
        PrimeField(2146559468229273184),
        PrimeField(12307437278508535873),
        PrimeField(2264793496591040620)};

    for (int i = 0; i < 10; i++)
    {
        PrimeField r = test_case[i];
        auto a_shares = qmpc::Share::PFtoBS(r);
        ShareComp ans = qmpc::Share::BStoSC(a_shares);
        open(ans);
        PrimeField recons_ans = recons(ans);
        if (recons_ans.getVal() != r.getVal())
        {
            flag = false;
            break;
        }
    }

    EXPECT_TRUE(flag);
}

// PrimeFieldをBitshareに正しく変換できているかチェック
TEST(ShareCompTest, PFtoBS)
{
    bool flag = true;  // テストが失敗した場合falseになる
    // テストケース
    PrimeField test_case[10] = {
        PrimeField(5801164259233537525),
        PrimeField(11990882371217920413),
        PrimeField(12133433930879247631),
        PrimeField(12367114231622338330),
        PrimeField(5019517851162398891),
        PrimeField(4135365588412164698),
        PrimeField(5721501916669856084),
        PrimeField(12279116850396682003),
        PrimeField(7702089081674672791),
        PrimeField(1083686755848627406)};

    for (int i = 0; i < 10; i++)
    {
        PrimeField r = test_case[i];
        auto a_shares = qmpc::Share::PFtoBS(r);
        boost::multiprecision::uint128_t x = r.getVal();
        for (int j = PrimeField::l - 1; j >= 0; j--)
        {
            open(a_shares[j]);
            PrimeField ans = recons(a_shares[j]);
            if (ans.getVal() != x % 2)
            {
                flag = false;
                break;
            }
            x = x / 2;
        }
        if (!flag)
        {
            break;
        }
    }

    EXPECT_TRUE(flag);
}

// a < p/2の場合は1, それ以外の場合は0であるかチェックする
TEST(ShareCompTest, SubHalfLessThan)
{
    // loop回テストを回す
    for (int loop = 0; loop < 10; loop++)
    {
        ShareComp a_share = qmpc::Share::subRNS();
        open(a_share);
        PrimeField a_rec = recons(a_share);

        ShareComp ret = qmpc::Share::subHalfLessThan(a_share);
        open(ret);
        PrimeField recons_ret = recons(ret);
        // デバッグ用出力
        spdlog::info("a_rec = \t{}", a_rec.getVal());
        spdlog::info("p = \t\t{}", (PrimeField::p / 2) + 1);
        spdlog::info("recons_ret = \t{}", recons_ret.getVal());

        // a < p/2 の場合は，recons_ret = 1
        // そうでない場合は，recons_ret = 0
        if (a_rec.getVal() < (PrimeField::p / 2) + 1)
        {
            EXPECT_EQ(recons_ret.getVal(), 1);
        }
        else
        {
            EXPECT_EQ(recons_ret.getVal(), 0);
        }
    }
}

// 以下、メインプロトコルのテスト

// c1 < a < c2の場合は1, そうでない場合は0を返す
TEST(ShareCompTest, IntervalTest)
{
    // Test1 (c1 < a < c2が成立する場合)
    PrimeField c1(9000000);
    PrimeField c2(10000000);
    std::vector<ShareComp> bit_shares = qmpc::Share::PFtoBS(PrimeField(9500000));
    ShareComp a_share = qmpc::Share::BStoSC(bit_shares);
    ShareComp interval_share = qmpc::Share::intervalTest(c1, a_share, c2);
    open(interval_share);
    PrimeField ret = recons(interval_share);
    spdlog::info("[TestInterval 1] ret = {}", ret.getVal());
    EXPECT_EQ(ret.getVal(), 1);
    // Test2 (c1 < a < c2が成立しない場合)
    bit_shares = qmpc::Share::PFtoBS(PrimeField(10500000));
    ShareComp b_share = qmpc::Share::BStoSC(bit_shares);
    ShareComp interval_share2 = qmpc::Share::intervalTest(c1, b_share, c2);
    open(interval_share2);
    ret = recons(interval_share2);
    spdlog::info("[TestInterval 2] ret = {}", ret.getVal());
    EXPECT_EQ(ret.getVal(), 0);
}

// a < bの場合は1, そうでない場合は0を返す
TEST(ShareCompTest, ComparisonTest)
{
    // Test1 (a < bが成立する場合)
    auto a_bit_shares = qmpc::Share::PFtoBS(PrimeField(9500000));
    auto b_bit_shares = qmpc::Share::PFtoBS(PrimeField(10500000));
    ShareComp a_share = qmpc::Share::BStoSC(a_bit_shares);
    ShareComp b_share = qmpc::Share::BStoSC(b_bit_shares);
    ShareComp comp_share = qmpc::Share::comparisonTest(a_share, b_share);
    open(comp_share);
    PrimeField ret = recons(comp_share);
    spdlog::info("[TestComparison 1] ret = {}", ret.getVal());
    EXPECT_EQ(ret.getVal(), 1);
    // Test2 (a < bが成立しない場合)
    auto c_bit_shares = qmpc::Share::PFtoBS(PrimeField(155000000));
    auto d_bit_shares = qmpc::Share::PFtoBS(PrimeField(145000000));
    ShareComp c_share = qmpc::Share::BStoSC(c_bit_shares);
    ShareComp d_share = qmpc::Share::BStoSC(d_bit_shares);
    ShareComp comp_share2 = qmpc::Share::comparisonTest(c_share, d_share);
    open(comp_share2);
    ret = recons(comp_share2);
    spdlog::info("[TestComparison 2] ret = {}", ret.getVal());
    EXPECT_EQ(ret.getVal(), 0);
}

// a = bの場合は1、そうでない場合は0を返す
TEST(ShareCompTest, EqualityTest)
{
    // Test1 (a = bが成立する場合)
    auto a_bit_shares = qmpc::Share::PFtoBS(PrimeField(10500000));
    auto b_bit_shares = qmpc::Share::PFtoBS(PrimeField(10500000));
    ShareComp a_share = qmpc::Share::BStoSC(a_bit_shares);
    ShareComp b_share = qmpc::Share::BStoSC(b_bit_shares);
    ShareComp e_share = qmpc::Share::equalityTest(a_share, b_share);
    open(e_share);
    PrimeField ret = recons(e_share);
    spdlog::info("[TestEquality 1] ret = {}", ret.getVal());
    EXPECT_EQ(ret.getVal(), 1);
    // Test2 (a = bが成立しない場合)
    auto c_bit_shares = qmpc::Share::PFtoBS(PrimeField(10500000));
    auto d_bit_shares = qmpc::Share::PFtoBS(PrimeField(10500001));
    ShareComp c_share = qmpc::Share::BStoSC(c_bit_shares);
    ShareComp d_share = qmpc::Share::BStoSC(d_bit_shares);
    ShareComp f_share = qmpc::Share::equalityTest(c_share, d_share);
    open(f_share);
    ret = recons(f_share);
    spdlog::info("[TestEquality 2] ret = {}", ret.getVal());
    EXPECT_EQ(ret.getVal(), 0);
}

//一括open,reconsテスト
TEST(ShareCompTest, ShareCompReconsBulk)
{
    Config *conf = Config::getInstance();
    int n_parties = conf->n_parties;
    std::vector<ShareComp> t = {PrimeField(1), PrimeField(2), PrimeField(3)};
    std::vector<PrimeField> expected = {
        PrimeField(n_parties), PrimeField(2 * n_parties), PrimeField(3 * n_parties)};
    open(t);
    auto target = recons(t);
    bool ng = false;

    for (size_t i = 0; i < t.size(); ++i)
    {
        if (expected[i] != target[i])
        {
            ng = true;
        }
    }

    EXPECT_TRUE(not ng);
}

//一括乗算のテスト
TEST(ShareCompTest, ShareCompMulBulk)
{
    Config *conf = Config::getInstance();
    int n_parties = conf->n_parties;
    std::vector<ShareComp> a = {PrimeField(1), PrimeField(2), PrimeField(3)};
    std::vector<ShareComp> b = {PrimeField(1), PrimeField(2), PrimeField(3)};
    std::vector<PrimeField> expected = {
        PrimeField(n_parties * n_parties),
        PrimeField((2 * n_parties) * (2 * n_parties)),
        PrimeField((3 * n_parties) * (3 * n_parties))};
    auto ret = a * b;
    open(ret);
    auto target = recons(ret);
    bool ng = false;

    spdlog::info("TestShareCompMulBulk End !!");
    for (size_t i = 0; i < a.size(); ++i)
    {
        spdlog::info(target[i].getStrVal());
        if (expected[i] != target[i])
        {
            ng = true;
        }
    }

    EXPECT_TRUE(not ng);
}
