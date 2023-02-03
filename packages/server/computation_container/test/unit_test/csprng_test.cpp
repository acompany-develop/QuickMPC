#include <stdlib.h>

#include <memory>
#include <set>

#include "logging/logger.hpp"
#include "random/csprng.hpp"
#include "gtest/gtest.h"

TEST(CsprngTest, GetRand)
{
    const unsigned int byteSize = 8;  // 8byte = 64bit

    std::unique_ptr<unsigned char[]> rnd = std::make_unique<unsigned char[]>(byteSize);

    Utility::CSPRNG rng = Utility::CSPRNG();
    rng.GetRand(rnd, byteSize);

    for (unsigned int i = 0; i < byteSize; i++)
    {
        unsigned int r = (unsigned int)rnd[i];
        QMPC_LOG_INFO("{:d}", r);
        ASSERT_TRUE((r >= 0) && (r <= 255));
    }
}

TEST(CsprngTest, GetRandLL)
{
    Utility::CSPRNG rng = Utility::CSPRNG();
    std::vector<long long int> rndLLVec = {};
    std::set<int> rndLLSet;

    for (unsigned int i = 0; i < 10000; i++)
    {
        long long int rndLL = rng.GetRandLL();
        rndLLVec.push_back(rndLL);
        rndLLSet.insert(rndLL);
    }

    // SetとVecのサイズで重複チェックテスト
    ASSERT_EQ(rndLLVec.size(), rndLLSet.size());
}

TEST(CsprngTest, GetRandLLVec)
{
    Utility::CSPRNG rng = Utility::CSPRNG();
    unsigned int vecSize = 1000;
    std::vector<long long int> rndLLVec = rng.GetRandLLVec(vecSize);
    std::set<int> rndLLSet(rndLLVec.begin(), rndLLVec.end());
    ;

    // SetとVecのサイズで重複チェックテスト
    ASSERT_EQ(rndLLVec.size(), rndLLSet.size());
}

TEST(CsprngTest, HowUse)
{
    Utility::CSPRNG rng = Utility::CSPRNG();

    const unsigned int byteSize = 8;  // 8byte = 64bit
    const unsigned int bitSize = byteSize * 8;

    std::unique_ptr<unsigned char[]> rnd = std::make_unique<unsigned char[]>(byteSize);
    rng.GetRand(rnd, byteSize);

    QMPC_LOG_INFO("変換前");
    QMPC_LOG_INFO("[bin] unsigned char*: ");
    std::string rnd_bin_str = "";
    for (unsigned int i = 0; i < byteSize; i++)
    {
        QMPC_LOG_INFO(std::bitset<8>(rnd[i]));
        std::stringstream str;
        str << std::bitset<8>(rnd[i]);
        rnd_bin_str += str.str();
    }

    QMPC_LOG_INFO("[hex] unsigned char*: ");
    for (unsigned int i = 0; i < byteSize; i++)
    {
        QMPC_LOG_INFO("{:x}", (unsigned int)rnd[i]);
    }

    QMPC_LOG_INFO("[dec] unsigned char*: ");
    for (unsigned int i = 0; i < byteSize; i++)
    {
        QMPC_LOG_INFO((unsigned int)rnd[i]);
    }
    QMPC_LOG_INFO("\n");

    QMPC_LOG_INFO("変換");
    QMPC_LOG_INFO("[bin] unsigned char* -> string: ");
    QMPC_LOG_INFO("{}\n", rnd_bin_str);

    // unsigned char* から long longへキャスト
    long long val = std::stoull(rnd_bin_str, nullptr, 2);
    unsigned long long uval = std::stoull(rnd_bin_str, nullptr, 2);

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
    [bin] unsigned char*:
    1010010010110000111000111010011111001100000010100110101001100010
    [hex] unsigned char*:
    a4 b0 e3 a7 cc a 6a 62
    [dec] unsigned char*:
    164 176 227 167 204 10 106 98

    変換
    [bin] unsigned char* -> string:
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
