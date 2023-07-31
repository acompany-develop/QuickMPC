#include "fixed_point/fixed_point.hpp"
#include "gtest/gtest.h"

TEST(FixedPointTest, Add)
{
    FixedPoint a("3.0");     // operand1
    FixedPoint b("0.141");   // operand2
    FixedPoint c("-0.141");  // operand3

    FixedPoint res1 = a + b;   // result
    FixedPoint cor1("3.141");  // correct
    EXPECT_EQ(res1, cor1);

    FixedPoint res2 = a + c;   // result
    FixedPoint cor2("2.859");  // correct
    EXPECT_EQ(res2, cor2);
}

TEST(FixedPointTest, AddAssign)
{
    FixedPoint a1("3.0");    // operand1
    FixedPoint a2("3.0");    // operand1
    FixedPoint b("0.141");   // operand2
    FixedPoint c("-0.141");  // operand3

    a1 += b;
    FixedPoint cor1("3.141");  // correct
    EXPECT_EQ(a1, cor1);

    a2 += c;
    FixedPoint cor2("2.859");  // correct
    EXPECT_EQ(a2, cor2);
}

TEST(FixedPointTest, Sub)
{
    FixedPoint a("3.141");   // operand1
    FixedPoint b("0.141");   // operand2
    FixedPoint c("-0.141");  // operand3

    FixedPoint res1 = a - b;  // result
    FixedPoint cor1("3.0");   // correct
    EXPECT_EQ(res1, cor1);

    FixedPoint res2 = a - c;   // result
    FixedPoint cor2("3.282");  // correct
    EXPECT_EQ(res2, cor2);
}

TEST(FixedPointTest, SubAssign)
{
    FixedPoint a1("3.141");  // operand1
    FixedPoint a2("3.141");  // operand1
    FixedPoint b("0.141");   // operand2
    FixedPoint c("-0.141");  // operand3

    a1 -= b;
    FixedPoint cor1("3.0");  // correct
    EXPECT_EQ(a1, cor1);

    a2 -= c;
    FixedPoint cor2("3.282");  // correct
    EXPECT_EQ(a2, cor2);
}

TEST(FixedPointTest, Mul)
{
    FixedPoint a("10.0");  // operand1
    FixedPoint b("0.1");   // operand2
    FixedPoint c("-0.1");  // operand3

    FixedPoint res1 = a * b;  // result
    FixedPoint cor1("1.0");   // correct
    EXPECT_EQ(res1, cor1);

    FixedPoint res2 = a * c;  // result
    FixedPoint cor2("-1.0");  // correct
    EXPECT_EQ(res2, cor2);
}

TEST(FixedPointTest, MulAssign)
{
    FixedPoint a1("10.0");  // operand1
    FixedPoint a2("10.0");  // operand1
    FixedPoint b("0.1");    // operand2
    FixedPoint c("-0.1");   // operand3

    a1 *= b;                 // result
    FixedPoint cor1("1.0");  // correct
    EXPECT_EQ(a1, cor1);

    a2 *= c;                  // result
    FixedPoint cor2("-1.0");  // correct
    EXPECT_EQ(a2, cor2);
}

TEST(FixedPointTest, Div)
{
    FixedPoint a("10.0");  // operand1
    FixedPoint b("0.1");   // operand2
    FixedPoint c("-0.1");  // operand3

    FixedPoint res1 = a / b;   // result
    FixedPoint cor1("100.0");  // correct
    EXPECT_EQ(res1, cor1);

    FixedPoint res2 = a / c;    // result
    FixedPoint cor2("-100.0");  // correct
    EXPECT_EQ(res2, cor2);
}

TEST(FixedPointTest, DivAssign)
{
    FixedPoint a1("10.0");  // operand1
    FixedPoint a2("10.0");  // operand1
    FixedPoint b("0.1");    // operand2
    FixedPoint c("-0.1");   // operand3

    a1 /= b;                   // result
    FixedPoint cor1("100.0");  // correct
    EXPECT_EQ(a1, cor1);

    a2 /= c;                    // result
    FixedPoint cor2("-100.0");  // correct
    EXPECT_EQ(a2, cor2);
}

TEST(FixedPointTest, LT)
{
    FixedPoint a("1.23");  // operand1
    FixedPoint b("1.45");  // operand2
    EXPECT_TRUE(a < b);
    EXPECT_FALSE(b < a);
    EXPECT_FALSE(a < a);

    FixedPoint c("-1.23");  // operand3
    FixedPoint d("-1.45");  // operand4
    EXPECT_FALSE(c < d);
    EXPECT_TRUE(d < c);
    EXPECT_FALSE(c < c);
}

TEST(FixedPointTest, LTE)
{
    FixedPoint a("1.23");  // operand1
    FixedPoint b("1.45");  // operand2
    EXPECT_TRUE(a <= b);
    EXPECT_FALSE(b <= a);
    EXPECT_TRUE(a <= a);

    FixedPoint c("-1.23");  // operand3
    FixedPoint d("-1.45");  // operand4
    EXPECT_FALSE(c <= d);
    EXPECT_TRUE(d <= c);
    EXPECT_TRUE(c <= c);
}

TEST(FixedPointTest, GT)
{
    FixedPoint a("1.23");  // operand1
    FixedPoint b("1.45");  // operand2
    EXPECT_FALSE(a > b);
    EXPECT_TRUE(b > a);
    EXPECT_FALSE(a > a);

    FixedPoint c("-1.23");  // operand3
    FixedPoint d("-1.45");  // operand4
    EXPECT_TRUE(c > d);
    EXPECT_FALSE(d > c);
    EXPECT_FALSE(c > c);
}

TEST(FixedPointTest, GTE)
{
    FixedPoint a("1.23");  // operand1
    FixedPoint b("1.45");  // operand2
    EXPECT_FALSE(a >= b);
    EXPECT_TRUE(b >= a);
    EXPECT_TRUE(a >= a);

    FixedPoint c("-1.23");  // operand3
    FixedPoint d("-1.45");  // operand4
    EXPECT_TRUE(c >= d);
    EXPECT_FALSE(d >= c);
    EXPECT_TRUE(c >= c);
}

TEST(FixedPointTest, Equation)
{
    FixedPoint a("1.45");  // operand1
    FixedPoint b("1.23");  // operand2
    EXPECT_TRUE(a == a);
    EXPECT_FALSE(a == b);

    FixedPoint c("-1.45");  // operand3
    FixedPoint d("-1.23");  // operand4
    EXPECT_TRUE(c == c);
    EXPECT_FALSE(c == d);
}

TEST(FixedPointTest, getVal)
{
    FixedPoint a("1.45");  // operand
    EXPECT_EQ(a.getStrVal(), "1.45");
    EXPECT_EQ(a.getDoubleVal(), 1.45);

    FixedPoint b("-1.45");  // operand
    EXPECT_EQ(b.getStrVal(), "-1.45");
    EXPECT_EQ(b.getDoubleVal(), -1.45);
}

TEST(FixedPointTest, Constructor)
{
    FixedPoint a{};
    EXPECT_EQ(a.getVal(), 0);

    FixedPoint b{3};
    EXPECT_NEAR(b.getDoubleVal(), 3, 0.0001);

    FixedPoint c{0.33333};
    EXPECT_NEAR(c.getDoubleVal(), 0.33333, 0.0001);

    FixedPoint d{"0.33333"};
    EXPECT_NEAR(d.getDoubleVal(), 0.33333, 0.0001);
}

TEST(FixedPointTest, Increment)
{
    FixedPoint a{};
    a++;
    EXPECT_EQ(a, FixedPoint(1));
}

TEST(FixedPointTest, Decrement)
{
    FixedPoint a{};
    a--;
    EXPECT_EQ(a, FixedPoint(-1));
}

TEST(FixedPointTest, Max)
{
    namespace mp = boost::multiprecision;
    mp::checked_int256_t ma =
        std::numeric_limits<mp::checked_int256_t>::max() / FixedPoint::getShift();
    FixedPoint a{ma};
    EXPECT_EQ(a, FixedPoint(ma));
}

TEST(FixedPointTest, Mod)
{
    FixedPoint a(10);
    FixedPoint b(2);
    EXPECT_EQ(a % b, FixedPoint(0));
}

TEST(FixedPointTest, OverInt256)
{
    namespace mp = boost::multiprecision;
    mp::checked_int256_t ma = std::numeric_limits<mp::checked_int256_t>::max();
    FixedPoint a{ma};
    a += FixedPoint{ma};

    EXPECT_TRUE(true);
}

TEST(FixedPointTest, Div0)
{
    try
    {
        FixedPoint a{1}, b{0};
        a /= b;
    }
    catch (std::exception e)
    {
        EXPECT_TRUE(true);  // succeed
        return;
    }
    EXPECT_TRUE(false);  // failed
}

TEST(FixedPointTest, Abs)
{
    FixedPoint a("1.45");
    EXPECT_EQ(qmpc::Utils::abs(a), a);

    FixedPoint b("-1.45");
    EXPECT_EQ(qmpc::Utils::abs(b), a);
}

TEST(FixedPointTest, Inf)
{
    try
    {
        FixedPoint a(1e301);
    }
    catch (const std::exception &e)
    {
        EXPECT_TRUE(false);  // failed
        return;
    }
    EXPECT_TRUE(true);  // succeed
}
