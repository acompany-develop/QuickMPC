#include <stdlib.h>

#include <iomanip>
#include <memory>
#include <set>

#include "gtest/gtest.h"
#include "logging/logger.hpp"
#include "random/csprng.hpp"

class test_sodium_seed
{
public:
    using seed_type = std::array<unsigned char, randombytes_SEEDBYTES>;
    test_sodium_seed() {}
    test_sodium_seed(const seed_type& const_seed) {}
    seed_type operator()() { return seed; }
    void set_seed(const seed_type& seed) { this->seed = seed; }

private:
    seed_type seed = {"abcdefgh"};
};
TEST(CsprngTest, GetRand)
{
    const std::uint32_t byteSize = 8;  // 8byte = 64bit

    std::unique_ptr<std::uint8_t[]> rnd = std::make_unique<std::uint8_t[]>(byteSize);

    Utility::CSPRNG rng = Utility::CSPRNG();
    rng.GetRand(rnd, byteSize);

    for (std::uint32_t i = 0; i < byteSize; i++)
    {
        std::uint32_t r = (std::uint32_t)rnd[i];
        QMPC_LOG_INFO("{:d}", r);
        ASSERT_TRUE((r >= 0) && (r <= 255));
    }
}

TEST(CsprngTest, GetRandLL)
{
    Utility::CSPRNG rng = Utility::CSPRNG();
    std::vector<std::int64_t> rndLLVec = {};
    std::set<std::int64_t> rndLLSet;
    for (std::uint32_t i = 0; i < 10'000; i++)
    {
        std::int64_t rndLL = rng.GetRandLL();
        rndLLVec.push_back(rndLL);
        rndLLSet.insert(rndLL);
    }

    // SetとVecのサイズで重複チェックテスト
    ASSERT_EQ(rndLLVec.size(), rndLLSet.size());
}

TEST(CsprngTest, GetRandLLVec)
{
    Utility::CSPRNG rng = Utility::CSPRNG();
    std::size_t vecSize = 1000;
    std::vector<std::int64_t> rndLLVec = rng.GetRandLLVec(vecSize);
    std::set<std::int64_t> rndLLSet(rndLLVec.begin(), rndLLVec.end());

    // SetとVecのサイズで重複チェックテスト
    ASSERT_EQ(rndLLVec.size(), rndLLSet.size());
}

TEST(CsprngTest, HowUse)
{
    Utility::CSPRNG rng = Utility::CSPRNG();

    const std::uint32_t byteSize = 8;  // 8byte = 64bit
    const std::uint32_t bitSize = byteSize * 8;

    std::unique_ptr<std::uint8_t[]> rnd = std::make_unique<std::uint8_t[]>(byteSize);
    rng.GetRand(rnd, byteSize);

    QMPC_LOG_INFO("変換前");
    QMPC_LOG_INFO("[bin] std::uint8_t*: ");
    std::string rnd_bin_str = "";
    for (std::uint32_t i = 0; i < byteSize; i++)
    {
        QMPC_LOG_INFO(std::bitset<8>(rnd[i]));
        std::stringstream str;
        str << std::bitset<8>(rnd[i]);
        rnd_bin_str += str.str();
    }

    QMPC_LOG_INFO("[hex] std::uint8_t*: ");
    for (std::uint32_t i = 0; i < byteSize; i++)
    {
        QMPC_LOG_INFO("{:x}", (std::uint32_t)rnd[i]);
    }

    QMPC_LOG_INFO("[dec] std::uint8_t*: ");
    for (std::uint32_t i = 0; i < byteSize; i++)
    {
        QMPC_LOG_INFO((std::uint32_t)rnd[i]);
    }
    QMPC_LOG_INFO("\n");

    QMPC_LOG_INFO("変換");
    QMPC_LOG_INFO("[bin] std::uint8_t* -> string: ");
    QMPC_LOG_INFO("{}\n", rnd_bin_str);

    // std::uint8_t* から std::int64_t へキャスト
    std::int64_t val = std::stoull(rnd_bin_str, nullptr, 2);
    std::uint64_t uval = std::stoull(rnd_bin_str, nullptr, 2);

    QMPC_LOG_INFO("変換後");
    std::stringstream str;
    str << std::bitset<bitSize>(val);
    std::string val_bin_str = str.str();
    str.str("");
    str.clear(std::stringstream::goodbit);
    str << std::bitset<bitSize>(uval);
    std::string uval_bin_str = str.str();
    str.str("");
    str.clear(std::stringstream::goodbit);

    QMPC_LOG_INFO("[bin] ll: ");
    QMPC_LOG_INFO(val_bin_str);
    QMPC_LOG_INFO("[bin] ull: ");
    QMPC_LOG_INFO(uval_bin_str);
    str << std::hex << val;
    std::string val_hex_str = str.str();
    str.str("");
    str.clear(std::stringstream::goodbit);
    str << std::hex << uval;
    std::string uval_hex_str = str.str();

    QMPC_LOG_INFO("[hex] ll: ");
    QMPC_LOG_INFO(val_hex_str);
    QMPC_LOG_INFO("[hex] ull: ");
    QMPC_LOG_INFO(uval_hex_str);

    QMPC_LOG_INFO("[dec] ll");
    QMPC_LOG_INFO("{:d}", val);
    QMPC_LOG_INFO("[dec] ull");
    QMPC_LOG_INFO(uval);

    // 元のrnd-ll-ull bin比較
    ASSERT_TRUE((rnd_bin_str == val_bin_str && rnd_bin_str == uval_bin_str));

    // ll-ull hex比較
    ASSERT_EQ(val_hex_str, uval_hex_str);

    /* 例
    変換前
    [bin] std::uint8_t*:
    1010010010110000111000111010011111001100000010100110101001100010
    [hex] std::uint8_t*:
    a4 b0 e3 a7 cc a 6a 62
    [dec] std::uint8_t*:
    164 176 227 167 204 10 106 98

    変換
    [bin] std::uint8_t* -> string:
    1010010010110000111000111010011111001100000010100110101001100010

    変換後
    [bin] ll:
    1010010010110000111000111010011111001100000010100110101001100010
    [bin] ull:
    1010010010110000111000111010011111001100000010100110101001100010
    [hex] ll:
    a4b0e3a7cc0a6a62
    [hex] ull:
    a4b0e3a7cc0a6a62
    [dec] ll
    -6579508745766016414
    [dec] ull
    11867235327943535202
    */
}

TEST(csprngtest, constvalue)
{
    qmpc::random::sodium_random<test_sodium_seed> const_sodium;
    qmpc::random::sodium_random<test_sodium_seed> const_sodium2;
    auto rand1 = const_sodium();
    auto rand2 = const_sodium2();
    std::cout << "test sodium value is " << rand1 << std::endl;
    ASSERT_EQ(rand1, rand2);

    auto vec1 = const_sodium(10);
    auto vec2 = const_sodium2(10);
    for (int i = 0; i < 10; ++i)
    {
        ASSERT_EQ(vec1[i], vec2[i]);
        std::cout << "test sodium1 value " << i << " " << vec1[i] << std::endl;
    }
    test_sodium_seed test;
    test_sodium_seed::seed_type seed = {"bcdfghij"};
    test.set_seed(seed);
    qmpc::random::sodium_random<test_sodium_seed> const_sodium3(test);
    auto rand3 = const_sodium3();

    std::cout << "test sodium value is " << rand3 << std::endl;

    auto vec3 = const_sodium3(10);
    for (int i = 0; i < 10; ++i)
    {
        ASSERT_NE(vec1[i], vec3[i]);
        std::cout << "test sodium3 value " << i << " " << vec3[i] << std::endl;
    }
}
