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

// class norandom_test : public qmpc::random::csprng_interface<norandom_test>
// {
// public:
//     using result_type = unsigned int;
//     inline static constexpr result_type C = 5;
//     auto generate()
//     {
//         result_type ret = C;
//         return ret;
//     }
//     auto generate(const size_t size)
//     {
//         std::vector<result_type> data(size);
//         for (int i = 0; i < size; ++i)
//         {
//             data[i] = C;
//         }
//         return data;
//     }
// };

// template <int CONST_VALUE>
// class const_seed : public qmpc::random::csprng_interface<const_seed<CONST_VALUE>>
// {
// public:
//     using result_type = unsigned char;
//     inline static constexpr result_type C = CONST_VALUE;
//     auto generate() { return static_cast<result_type>(C); }
//     auto generate(size_t n)
//     {
//         std::vector<unsigned char> seed(n, static_cast<result_type>(C));
//         return seed;
//     }
// };
// TEST(RandomTest, newRandomTest)
// {
//     random_csprng<qmpc::random::sodium_random<>> random;
//     for (int i = 0; i < 10; ++i)
//     {
//         std::cout << "new random value is " << random.get<int>(0, 100) << std::endl;
//     }

//     std::set<unsigned long long> s;
//     for (int i = 0; i < 1'000'000; ++i)
//     {
//         s.insert(random.get<unsigned long long>());
//     }
//     std::cout << "random set 1'000'000 /" << s.size() << std::endl;

//     auto vec = random.get_array<unsigned long long>(1'000'000);
//     std::set<unsigned long long> s2;
//     for (auto& a : vec)
//     {
//         s2.insert(a);
//     }
//     std::cout << "random set 1'000'000 / " << s2.size() << std::endl;
// }
// TEST(RandomTest, constCSprng)
// {
//     random_csprng<qmpc::random::sodium_random<const_seed<50>>> random;

//     random_csprng<qmpc::random::sodium_random<const_seed<50>>> random2;
//     for (int i = 0; i < 10; ++i)
//     {
//         auto tmp1 = random.get<int>(0, 100);
//         auto tmp2 = random.get<int>(0, 100);
//         ASSERT_EQ(tmp1, tmp2);
//     }
// }

// TEST(csprngtest, interfaceDefaultUse)
// {
//     using namespace std;
//     unsigned long long ma = std::numeric_limits<unsigned long long>::max();
//     std::uniform_int_distribution<unsigned long long> dist(0, ma);
//     for (int i = 0; i < 10; ++i)
//     {
//         qmpc::random::sodium_random random;
//         auto value = dist(random);
//         EXPECT_GE(value, 0);
//         EXPECT_LE(value, ma);
//     }
//     double dma = std::numeric_limits<double>::max();
//     std::uniform_real_distribution<double> dist_d(0.0, dma);
//     for (int i = 0; i < 10; ++i)
//     {
//         qmpc::random::sodium_random random;
//         auto value = dist_d(random);
//         EXPECT_GE(value, 0);
//         EXPECT_LE(value, dma);
//     }
// }

// TEST(csprngtest, norandom)
// {
//     norandom_test random;

//     auto ret = random();
//     auto ret2 = random();
//     ASSERT_EQ(ret, ret2);
//     ASSERT_EQ(ret, norandom_test::C);

//     auto vec = random(10);
//     auto vec2 = random(10);
//     for (int i = 0; i < 10; ++i)
//     {
//         ASSERT_EQ(vec[i], norandom_test::C);
//         ASSERT_EQ(vec2[i], norandom_test::C);
//     }
// }