#include "random/csprng.hpp"

#include <memory>

#include "logging/logger.hpp"
// randombytes_buf_deterministic の内部はChaCha20
// ((2**32[ctrが4byte長])*64[byte/block])/10**9[byte->GB] = 274GB[俗にいう256GB]
// 内部ctrの範囲が1~2**32なので、1つのkey(seed)で274GB分のRNが生成できる

namespace Utility
{
bool CSPRNG::entropyCheck()
{
    while (sodium_init() < 0)
    {
        static std::uint16_t ctr = 0;
        ctr++;
        if (ctr > 3000)  // エントロピーが出来上がるのに最大3分程度待つ
        {
            QMPC_LOG_INFO("There is a serious lack of entropy.");
            // 例外を送出
            // throw "There is a serious lack of entropy.";
            return false;
        }
        else if (ctr % 1000 == 0)  // 10秒溜まってなかったら1分待つ
        {
            // あまりにもエントロピーがたまらないので1分待機
            QMPC_LOG_INFO("Standing by for one minute due to lack of entropy...");
            std::this_thread::sleep_for(std::chrono::minutes(1));  // 1分待機
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(3));  // 3 マイクロ秒待機
    }
    return true;
};

void CSPRNG::GetRand(const std::unique_ptr<std::uint8_t[]>& buf, const std::size_t byteSize)
{
    if (!this->CSPRNG::entropyCheck())
    {
        throw "There is a serious lack of entropy.";
        qmpc::Log::throw_with_trace(
            std::runtime_error("RandomError: There is a serious lack of entropy.")
        );
    }
    if (byteSize
        <= 32)  // seedで32byteの乱数を使うので、seed以下のサイズの場合はseedを乱数として返す。
    {
        randombytes_buf(buf.get(), byteSize);
    }
    else
    {
        std::unique_ptr<std::uint8_t[]> seed =
            std::make_unique<std::uint8_t[]>(randombytes_SEEDBYTES);
        syscall(SYS_getrandom, seed.get(), randombytes_SEEDBYTES, 1);
        randombytes_buf_deterministic(buf.get(), byteSize, seed.get());
    }
};

std::int64_t CSPRNG::GetRandLL()
{
    constexpr std::size_t LL_SIZE = sizeof(std::int64_t);  // 8byte = 64bit
    constexpr std::size_t BYTE_SIZE = 8;                   // 8bit

    std::unique_ptr<std::uint8_t[]> rnd = std::make_unique<std::uint8_t[]>(LL_SIZE);
    // 64bit乱数[std::uint8_t*]生成
    this->CSPRNG::GetRand(rnd, LL_SIZE);

    // uint8_t* -> str(bin)
    std::int64_t rndVal = 0;
    for (std::size_t i = 0; i < LL_SIZE; i++)
    {
        rndVal =
            (rndVal << BYTE_SIZE) + *reinterpret_cast<std::int64_t *>(&rnd[i]);
    }
    return rndVal;
};

std::vector<std::int64_t> CSPRNG::GetRandLLVec(const std::size_t size)
{
    std::vector<std::int64_t> randLLVec = {};

    constexpr std::size_t LL_SIZE = sizeof(std::int64_t);  // 8byte
    constexpr std::size_t BYTE_SIZE = 8;                   // 8bit
    const std::size_t byteSize = size * LL_SIZE;           // size * 8[byte/llsize]

    std::unique_ptr<std::uint8_t[]> rnd = std::make_unique<std::uint8_t[]>(byteSize);
    // 64bit乱数[std::uint8_t*]生成
    this->CSPRNG::GetRand(rnd, byteSize);

    // unit8_t* -> str(bin)
    for (std::size_t i = 0; i < byteSize; i += LL_SIZE)
    {
        std::int64_t rndVal = 0;
        for (std::size_t j = 0; j < LL_SIZE; j++)
        {
            rndVal = (rndVal << BYTE_SIZE) +
                     *reinterpret_cast<std::int64_t *>(&rnd[i + j]);
        }
        randLLVec.push_back(rndVal);
    }

    return randLLVec;
};
}  // namespace Utility
