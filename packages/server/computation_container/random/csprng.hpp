#ifndef CSPRNG_H_
#define CSPRNG_H_

#include <linux/random.h>
#include <sodium.h>
#include <sys/random.h>
#include <sys/syscall.h>
#include <unistd.h>

#include <bitset>
#include <chrono>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <thread>
#include <vector>
#include "random/csprng_interface.hpp"

namespace Utility
{
class CSPRNG
{
    bool entropyCheck();
    CSPRNG &operator=(const CSPRNG &rhs);  // 代入演算子。代入を想定しないので禁止。
    CSPRNG(const CSPRNG &rhs);  // コピーコンストラクタ。コピーを想定しないので禁止。
public:
    CSPRNG(){};
    ~CSPRNG(){};

    // size is the byte size of random (ex. 128bit -> size is 16)
    // byteSizeが256bit以下だと/dev/urandomを使用
    void GetRand(const std::unique_ptr<std::uint8_t[]> &buf, const std::size_t byteSize);
    std::int64_t GetRandLL();
    std::vector<std::int64_t> GetRandLLVec(const std::size_t size);
};
}  // namespace Utility

#endif