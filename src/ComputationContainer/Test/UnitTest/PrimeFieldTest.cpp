#include <iostream>

#include "PrimeField/PrimeField.hpp"
#include "gtest/gtest.h"

constexpr boost::multiprecision::uint128_t p = 18446744073709551557ull;
constexpr boost::multiprecision::uint128_t x = 10000000;
constexpr boost::multiprecision::uint128_t x_minus = 18446744073699551557ull;  // x_minus = p-x
constexpr boost::multiprecision::uint128_t y = 20000000;
constexpr boost::multiprecision::uint128_t y_inv = 11710276295512354770ull;  // y_inv = y^-1

// コンストラクタのチェック
// メンバ変数に正しい値が入っているかテスト
TEST(PrimeFieldTest, Constructor)
{
    PrimeField x1;
    PrimeField x2(x);
    EXPECT_EQ(x1.getVal(), 0);
    EXPECT_EQ(x2.getVal(), x);
    EXPECT_EQ(PrimeField::p, 18446744073709551557ull);
    EXPECT_EQ(PrimeField::l, 64);
    EXPECT_EQ(PrimeField::lambda, 8);
    EXPECT_EQ(PrimeField::inverse_of_two, 9223372036854775779ull);
}

// 逆元が取得できるかチェック
TEST(PrimeFieldTest, GetInv)
{
    auto yp = PrimeField(y);
    EXPECT_EQ(yp.getInv().getVal(), y_inv);
}

// 代入演算子 = のテスト. ans = x の代入結果が ans == x になればOK
TEST(PrimeFieldTest, OpAssign)
{
    auto xp = PrimeField(x);
    PrimeField ans = xp;
    EXPECT_EQ(xp, ans);
}

// 単項演算子 - のテスト. -x の正しい結果が ans
TEST(PrimeFieldTest, OpMinus)
{
    auto xp = PrimeField(x);
    auto ans = PrimeField(x_minus);
    EXPECT_EQ((-xp), ans);
}

// 足し算 + のテスト. x + y の正しい結果が ans
TEST(PrimeFieldTest, OpAdd)
{
    auto xp = PrimeField(x);
    auto yp = PrimeField(y);
    auto ans = PrimeField(x + y);
    EXPECT_EQ(xp + yp, ans);
}

// 引き算 - のテスト. x - y の正しい結果が ans
TEST(PrimeFieldTest, OpSub)
{
    auto xp = PrimeField(x);
    auto yp = PrimeField(y);
    auto ans = PrimeField(x - y + p);
    EXPECT_EQ(xp - yp, ans);
}

// 掛け算 * のテスト. x * y の正しい結果が ans
TEST(PrimeFieldTest, OpMul)
{
    auto xp = PrimeField(x);
    auto yp = PrimeField(y);
    auto ans = PrimeField(x * y);
    EXPECT_EQ(xp * yp, ans);
}

// 割り算 / のテスト. x / y の正しい結果が ans
TEST(PrimeFieldTest, OpDiv)
{
    auto xp = PrimeField(x);
    auto yp = PrimeField(y);
    auto ans = PrimeField(x * y_inv % p);
    EXPECT_EQ(xp / yp, ans);
}

// 剰余算 % のテスト. y % x の正しい結果が ans
TEST(PrimeFieldTest, OpMod)
{
    auto xp = PrimeField(x);
    auto yp = PrimeField(y);
    auto ans = PrimeField(y % x);
    EXPECT_EQ(yp % xp, ans);
}

// 複合代入演算子 += のテスト. x += y の正しい結果が ans
TEST(PrimeFieldTest, OpAddAssign)
{
    auto xp = PrimeField(x);
    xp += PrimeField(y);
    auto ans = PrimeField(x + y);
    EXPECT_EQ(xp, ans);
}

// 複合代入演算子 -= のテスト. x -= y の正しい結果が ans
TEST(PrimeFieldTest, OpSubAssign)
{
    auto xp = PrimeField(x);
    xp -= PrimeField(y);
    auto ans = PrimeField(x - y + p);
    EXPECT_EQ(xp, ans);
}

// 複合代入演算子 *= のテスト. x *= y の正しい結果が ans
TEST(PrimeFieldTest, OpMulAssign)
{
    auto xp = PrimeField(x);
    xp *= PrimeField(y);
    auto ans = PrimeField(x * y);
    EXPECT_EQ(xp, ans);
}

// 複合代入演算子 /= のテスト. x /= y の正しい結果が ans
TEST(PrimeFieldTest, OpDivAssign)
{
    auto xp = PrimeField(x);
    xp /= PrimeField(y);
    auto ans = PrimeField(x * y_inv % p);
    EXPECT_EQ(xp, ans);
}

// 複合代入演算子 %= のテスト. y %= x の正しい結果が ans
TEST(PrimeFieldTest, OpModAssign)
{
    auto yp = PrimeField(y);
    yp %= PrimeField(x);
    auto ans = PrimeField(y % x);
    EXPECT_EQ(yp, ans);
}

// 比較演算子 == のテスト. x == y の正しい結果が ans
TEST(PrimeFieldTest, OpEq)
{
    auto xp = PrimeField(x);
    auto ans = PrimeField(x);
    EXPECT_EQ(xp, ans);
}

// 比較演算子 < のテスト. x < y の正しい結果が ans
TEST(PrimeFieldTest, OpLt)
{
    auto xp = PrimeField(x);
    auto yp = PrimeField(y);
    auto ans_lt = xp < yp;
    auto ans_gt = yp < xp;
    auto ans_eq = xp < xp;
    EXPECT_TRUE(ans_lt);
    EXPECT_FALSE(ans_gt);
    EXPECT_FALSE(ans_eq);
}

// 比較演算子 > のテスト. x > y の正しい結果が ans
TEST(PrimeFieldTest, OpGt)
{
    auto xp = PrimeField(x);
    auto yp = PrimeField(y);
    auto ans_lt = xp > yp;
    auto ans_gt = yp > xp;
    auto ans_eq = xp > xp;
    EXPECT_FALSE(ans_lt);
    EXPECT_TRUE(ans_gt);
    EXPECT_FALSE(ans_eq);
}

// 比較演算子 <= のテスト. x <= y の正しい結果が ans
TEST(PrimeFieldTest, OpLte)
{
    auto xp = PrimeField(x);
    auto yp = PrimeField(y);
    auto ans_lt = xp <= yp;
    auto ans_gt = yp <= xp;
    auto ans_eq = xp <= xp;
    EXPECT_TRUE(ans_lt);
    EXPECT_FALSE(ans_gt);
    EXPECT_TRUE(ans_eq);
}

// 比較演算子 >= のテスト. x >= y の正しい結果が ans
TEST(PrimeFieldTest, OpGte)
{
    auto xp = PrimeField(x);
    auto yp = PrimeField(y);
    auto ans_lt = xp >= yp;
    auto ans_gt = yp >= xp;
    auto ans_eq = xp >= xp;
    EXPECT_FALSE(ans_lt);
    EXPECT_TRUE(ans_gt);
    EXPECT_TRUE(ans_eq);
}

// modを超えて加算するテスト
TEST(PrimeFieldTest, OverADD)
{
    PrimeField a{PrimeField::p - 1}, b{2};
    EXPECT_EQ(a + b, PrimeField(1));
}

// 0を下回って減算するテスト
TEST(PrimeFieldTest, OverSub)
{
    PrimeField a{1}, b{2};
    EXPECT_EQ(a - b, PrimeField(PrimeField::p - 1));
}

// modを超えて乗算するテスト
TEST(PrimeFieldTest, OverMul)
{
    PrimeField a{PrimeField::p - 1}, b{2};
    EXPECT_EQ(a * b, PrimeField(PrimeField::p - 2));
}

// modを超えた値での初期化でエラーするかテスト
TEST(PrimeFieldTest, InitOverValue)
{
    EXPECT_ANY_THROW([]() { PrimeField a{PrimeField::p + 1}; }());
}

// 負の値での初期化でエラーするかテスト
TEST(PrimeFieldTest, InitMinusValue)
{
    EXPECT_ANY_THROW([]() { PrimeField a{-1}; }());
}
