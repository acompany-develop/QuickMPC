#include <iostream>
#include <type_traits>

#include "fixed_point/fixed_point.hpp"
#include "gtest/gtest.h"
#include "random/random.hpp"

TEST(RandomTest, getRandTypeTest)
{
    auto instance = RandGenerator::getInstance();

    auto ll = instance->getRand<long long>();
    auto fp = instance->getRand<FixedPoint>();

    bool ok_ll = std::is_same<decltype(ll), long long>::value;
    bool ok_fp = std::is_same<decltype(fp), FixedPoint>::value;

    EXPECT_TRUE(ok_ll);
    EXPECT_TRUE(ok_fp);
}

TEST(RandomTest, getRandMinMaxTest)
{
    auto instance = RandGenerator::getInstance();

    for (int i = 0; i < 1000; ++i)
    {
        auto rnd_ll = instance->getRand<long long>(0, 10);
        EXPECT_GE(rnd_ll, 0);
        EXPECT_LE(rnd_ll, 10);

        auto rnd_fp = instance->getRand<FixedPoint>(0, 10);
        EXPECT_GE(rnd_fp, 0);
        EXPECT_LE(rnd_fp, 10);
    }
}

TEST(RandomTest, newRandomTest) { random_csprng<qmpc::random::sodium_random<>> random; }