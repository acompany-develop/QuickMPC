#include <iostream>
#include <type_traits>

#include "FixedPoint/FixedPoint.hpp"
#include "PrimeField/PrimeField.hpp"
#include "Random/Random.hpp"
#include "gtest/gtest.h"
TEST(RandomTest, getRandTypeTest)
{
    auto instance = RandGenerator::getInstance();

    auto ll = instance->getRand<long long>();
    auto fp = instance->getRand<FixedPoint>();
    auto pf = instance->getRand<PrimeField>();

    bool ok_ll = std::is_same<decltype(ll), long long>::value;
    bool ok_fp = std::is_same<decltype(fp), FixedPoint>::value;
    bool ok_pf = std::is_same<decltype(pf), PrimeField>::value;

    EXPECT_TRUE(ok_ll);
    EXPECT_TRUE(ok_fp);
    EXPECT_TRUE(ok_pf);
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

        auto rnd_pf = instance->getRand<PrimeField>(0, 10);
        EXPECT_GE(rnd_pf, 0);
        EXPECT_LE(rnd_pf, 10);
    }
}