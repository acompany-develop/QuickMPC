#include "Logging/Logger.hpp"
#include "Share/Share.hpp"
#include "gtest/gtest.h"

TEST(ShareTest, GetValue)
{
    Share a(FixedPoint("3.0"));
    EXPECT_EQ(a.getVal(), FixedPoint("3.0"));
}

TEST(ShareTest, IncrementId)
{
    Share a(FixedPoint("1.0"));
    Share b(FixedPoint("1.0"));
    int diff = b.getId().getShareId() - a.getId().getShareId();
    EXPECT_EQ(diff, 1);
}

TEST(ShareTest, AddBetweenShares)
{
    Share a(FixedPoint("3.0"));
    Share b(FixedPoint("0.141"));
    Share result = a + b;
    EXPECT_EQ(result.getVal(), FixedPoint("3.141"));
}

TEST(ShareTest, SubBetweenShares)
{
    Share a(FixedPoint("3.141"));
    Share b(FixedPoint("0.141"));
    Share result = a - b;
    EXPECT_EQ(result.getVal(), FixedPoint("3.0"));
}

TEST(ShareTest, AddBetweenShareAndScalar)
{
    Share a(FixedPoint("3.0"));
    FixedPoint b("0.141");
    Share result = a + b;
    EXPECT_EQ(result.getVal(), FixedPoint("3.141"));
}

TEST(ShareTest, SubBetweenShareAndScalar)
{
    Share a(FixedPoint("3.141"));
    FixedPoint b("0.141");
    Share result = a - b;
    EXPECT_EQ(result.getVal(), FixedPoint("3.0"));
}

TEST(ShareTest, MulBetweenShareAndScalar)
{
    Share a(FixedPoint("0.1"));
    FixedPoint b("10.0");
    Share result = a * b;
    EXPECT_EQ(result.getVal(), FixedPoint("1.0"));
}

TEST(ShareTest, DivBetweenShareAndScalar)
{
    Share a(FixedPoint("1.0"));
    FixedPoint b("10.0");
    Share result = a / b;
    EXPECT_EQ(result.getVal(), FixedPoint("0.1"));
}

TEST(ShareTest, GetVarStrOfShare)
{
    FixedPoint a("12.2"), b("1.355");
    Share sa(a), sb(b);
    Share result = sa + sb;
    EXPECT_EQ(result.getVal(), FixedPoint("13.555"));
}

TEST(ShareTest, GetVarDoubleOfShare)
{
    FixedPoint a("12.2"), b("1.355");
    Share sa(a), sb(b);
    Share result = sa + sb;
    QMPC_LOG_INFO("{}", result.getDoubleVal());
    EXPECT_NEAR(result.getDoubleVal(), 13.555, 0.001);
}