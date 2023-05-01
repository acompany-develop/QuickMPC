#include <stdlib.h>

#include <iomanip>
#include <memory>
#include <set>

#include "gtest/gtest.h"
#include "logging/logger.hpp"
#include "random/csprng.hpp"

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

TEST(csprngtest, interface)
{
    using namespace std;
    qmpc::random::sodium_random random;
    std::uniform_int_distribution<unsigned long long> dist1(0, 1000000);
    std::vector<qmpc::random::sodium_random::result_type> vec;
    for (int i = 0; i < 1'000; ++i)
    {
        vec.emplace_back(dist1(random));
    }
    for (int i = 0; i < 50; ++i)
    {
        std::cout << "clamp " << random.clamp<unsigned int>(0, 10) << std::endl;
    }

    std::cout << resetiosflags(ios_base::floatfield);

    auto vec1 = random(5);
    std::cout << "vec1 is " << vec1.size() << std::endl;
    for (auto&& a : vec1)
    {
        std::cout << "random vec is " << a << std::endl;
    }
}
TEST(csprngtest, interfaceDefault)
{
    using namespace std;
    qmpc::random::sodium_random random;
    std::uniform_int_distribution<unsigned long long> dist(0, 10'000'000'000'000);
    for (int i = 0; i < 10; ++i)
    {
        std::cout << dist(random) << std::endl;
    }

    std::uniform_real_distribution<double> dist_d(0.0, 100000.0);
    for (int i = 0; i < 10; ++i)
    {
        std::cout << fixed;
        std::cout << std::setprecision(10);
        std::cout << dist_d(random) << std::endl;
    }
    cout << resetiosflags(ios_base::floatfield);
}
class csprng_random_test : public qmpc::random::csprng_interface<csprng_random_test>
{
    std::array<unsigned char, randombytes_SEEDBYTES> seed;

public:
    using result_type = unsigned int;
    csprng_random_test()
    {
        if (sodium_init() == -1)
        {
            qmpc::Log::throw_with_trace(
                std::runtime_error("RandomError: There is a serious lack of entropy.")
            );
        };
    }
    csprng_random_test(const std::array<unsigned char, randombytes_SEEDBYTES>& seed)
    {
        if (sodium_init() == -1)
        {
            qmpc::Log::throw_with_trace(
                std::runtime_error("RandomError: There is a serious lack of entropy.")
            );
        };

        this->seed = seed;
    }
    auto generate()
    {
        result_type ret{};
        // TODO: remove
        randombytes_buf_deterministic(&ret, sizeof(result_type), seed.data());

        return ret;
    }
    auto generate(const size_t size)
    {
        std::vector<result_type> data(size);

        // TODO: remove
        randombytes_buf_deterministic(data.data(), sizeof(result_type) * size, seed.data());

        // TODO: replace this code.
        // randombytes_buf(data.data(), size);

        return data;
    }
};

TEST(csprngtest, constvalue)
{
    std::array<unsigned char, randombytes_SEEDBYTES> seed = {"abcdefghijklmn"};
    csprng_random_test random(seed);
    auto vec = random(10);
    auto vec2 = random(10);
    for (int i = 0; i < 10; ++i)
    {
        std::cout << "deterministic first is " << vec[i] << " second " << vec2[i] << std::endl;
    }

    std::cout << "clamp " << random.clamp<unsigned int>(0, 10) << std::endl;
    std::cout << "clamp " << random.clamp<unsigned int>(0, 10) << std::endl;

    std::cout << "clamp double " << random.clamp<double>(0, 10) << std::endl;
    std::cout << "clamp " << random.clamp<double>(0, 10) << std::endl;
}
