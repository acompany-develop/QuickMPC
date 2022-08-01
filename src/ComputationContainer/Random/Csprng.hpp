#ifndef CSPRNG_H_
#define CSPRNG_H_

#include <unistd.h>
#include <sys/syscall.h>
#include <linux/random.h>

#include <sodium.h>
#include <sys/random.h>

#include <chrono>
#include <thread>
#include <iostream>

#include <bitset>
#include <string>

#include <sstream>
#include <vector>

namespace Utility
{
    class CSPRNG
    {
        bool entropyCheck();
        CSPRNG &operator=(const CSPRNG &rhs); // 代入演算子。代入を想定しないので禁止。
        CSPRNG(const CSPRNG &rhs);            // コピーコンストラクタ。コピーを想定しないので禁止。
    public:
        CSPRNG(){};
        ~CSPRNG(){};

        // size is the byte size of random (ex. 128bit -> size is 16)
        // byteSizeが256bit以下だと/dev/urandomを使用
        void GetRand(unsigned char *buf, unsigned int byteSize);
        long long int GetRandLL();
        std::vector<long long int> GetRandLLVec(unsigned int size);
    };
} // namespace Utility

#endif