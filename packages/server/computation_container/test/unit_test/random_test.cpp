#include <iostream>
#include <type_traits>

#include <set>
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

class norandom_test : public qmpc::random::csprng_interface<norandom_test>
{
public:
    using result_type = unsigned int;
    inline static constexpr result_type C = 5;
    auto generate()
    {
        result_type ret = C;
        return ret;
    }
    auto generate(const size_t size)
    {
        std::vector<result_type> data(size);
        for (int i = 0; i < size; ++i)
        {
            data[i] = C;
        }
        return data;
    }
};

template <int CONST_VALUE>
class const_seed : public qmpc::random::csprng_interface<const_seed<CONST_VALUE>>
{
public:
    using result_type = unsigned char;
    inline static constexpr result_type C = CONST_VALUE;
    auto generate() { return static_cast<result_type>(C); }
    auto generate(size_t n)
    {
        std::vector<unsigned char> seed(n, static_cast<result_type>(C));
        return seed;
    }
};
TEST(RandomTest, newRandomTest)
{
    random_csprng<qmpc::random::sodium_random<>> random;
    for (int i = 0; i < 100; ++i)
    {
        auto value = random.get<int>(0, 1'000'000);
        EXPECT_GE(value, 0);
        EXPECT_LE(value, 1'000'000);
    }
    for (int i = 0; i < 100; ++i)
    {
        auto value = random.get<unsigned long long>(0, 1'000'000'000);
        EXPECT_GE(value, 0);
        EXPECT_LE(value, 1'000'000'000);
    }
}
TEST(RandomTest, constCSprng)
{
    random_csprng<qmpc::random::sodium_random<const_seed<50>>> random;

    random_csprng<qmpc::random::sodium_random<const_seed<50>>> random2;
    for (int i = 0; i < 10000; ++i)
    {
        auto tmp1 = random.get<int>(0, 100);
        auto tmp2 = random2.get<int>(0, 100);
        ASSERT_EQ(tmp1, tmp2);
    }
}