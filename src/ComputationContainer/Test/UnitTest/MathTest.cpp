#include <vector>

#include "LogHeader/Logger.hpp"
#include "Math/Math.hpp"
#include "gtest/gtest.h"

TEST(MathTest, MeanBetweenShares)
{
    Share a(FixedPoint("1.0"));
    Share b(FixedPoint("2.0"));
    Share c(FixedPoint("3.0"));

    std::vector<Share> v(3);
    v[0] = a;
    v[1] = b;
    v[2] = c;

    Share avg;  // 6.0 / 3.0 -> 2.0
    FixedPoint expected("2.0");

    avg = qmpc::Math::smean(v);
    spdlog::info(avg.getVal().getStrVal());

    EXPECT_NEAR(expected.getDoubleVal(), avg.getDoubleVal(), 0.01);
}