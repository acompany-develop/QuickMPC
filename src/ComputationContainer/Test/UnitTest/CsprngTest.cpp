#include <stdlib.h>

#include <set>

#include "LogHeader/Logger.hpp"
#include "Random/Csprng.hpp"
#include "gtest/gtest.h"

TEST(CsprngTest, GetRand)
{
    const unsigned int byteSize = 8;  // 8byte = 64bit

    unsigned char *rnd = new unsigned char[byteSize];

    Utility::CSPRNG rng = Utility::CSPRNG();
    rng.GetRand(rnd, byteSize);

    for (unsigned int i = 0; i < byteSize; i++)
    {
        unsigned int r = (unsigned int)rnd[i];
        spdlog::info("{:d}", r);
        ASSERT_TRUE((r >= 0) && (r <= 255));
    }
    delete[] rnd;
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

    unsigned char *rnd = new unsigned char[byteSize];
    rng.GetRand(rnd, byteSize);

    spdlog::info("変換前");
    spdlog::info("[bin] unsigned char*: ");
    std::string rnd_bin_str = "";
    for (unsigned int i = 0; i < byteSize; i++)
    {
        spdlog::info(std::bitset<8>(rnd[i]));
        std::stringstream str;
        str << std::bitset<8>(rnd[i]);
        rnd_bin_str += str.str();
    }

    spdlog::info("[hex] unsigned char*: ");
    for (unsigned int i = 0; i < byteSize; i++)
    {
        spdlog::info("{:x}", (unsigned int)rnd[i]);
    }

    spdlog::info("[dec] unsigned char*: ");
    for (unsigned int i = 0; i < byteSize; i++)
    {
        spdlog::info((unsigned int)rnd[i]);
    }
    spdlog::info("\n");
    delete[] rnd;

    spdlog::info("変換");
    spdlog::info("[bin] unsigned char* -> string: ");
    spdlog::info("{}\n", rnd_bin_str);

    // unsigned char* から long longへキャスト
    long long val = std::stoull(rnd_bin_str, nullptr, 2);
    unsigned long long uval = std::stoull(rnd_bin_str, nullptr, 2);

    spdlog::info("変換後");
    std::stringstream str;
    str << std::bitset<bitSize>(val);
    std::string val_bin_str = str.str();
    str.str("");
    str.clear(std::stringstream::goodbit);
    str << std::bitset<bitSize>(uval);
    std::string uval_bin_str = str.str();
    str.str("");
    str.clear(std::stringstream::goodbit);

    spdlog::info("[bin] ll: ");
    spdlog::info(val_bin_str);
    spdlog::info("[bin] ull: ");
    spdlog::info(uval_bin_str);
    str << std::hex << val;
    std::string val_hex_str = str.str();
    str.str("");
    str.clear(std::stringstream::goodbit);
    str << std::hex << uval;
    std::string uval_hex_str = str.str();

    spdlog::info("[hex] ll: ");
    spdlog::info(val_hex_str);
    spdlog::info("[hex] ull: ");
    spdlog::info(uval_hex_str);

    spdlog::info("[dec] ll");
    spdlog::info("{:d}", val);
    spdlog::info("[dec] ull");
    spdlog::info(uval);

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
