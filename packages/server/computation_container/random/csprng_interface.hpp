#pragma once
#include <sodium.h>
#include <bitset>
#include <limits>
#include <random>
#include "logging/logger.hpp"
namespace qmpc::random
{

template <typename PRNG>
struct csprng_interface
{
public:
    template <typename T = PRNG>
    static constexpr typename T::result_type max()
    {
        return std::numeric_limits<typename T::result_type>::max();
    }
    template <typename T = PRNG>
    static constexpr typename T::result_type min()
    {
        return std::numeric_limits<typename T::result_type>::min();
    }
    /**
     * @brief 乱数生成関数
     *
     * @return result_type型の乱数
     */
    auto operator()() { return static_cast<PRNG*>(this)->generate(); }
    /**
     * @brief 乱数ベクターを返す関数
     *
     * @param n サイズ
     * @param seed 乱数生成用シード
     * @return value_type型のvectorを返す関数へのフック
     */
    auto operator()(size_t n) { return static_cast<PRNG*>(this)->generate(n); }
};

class sodium_random : public csprng_interface<sodium_random>
{
public:
    using result_type = unsigned int;
    using seed_type = std::array<unsigned char, randombytes_SEEDBYTES>;
    static std::array<unsigned char, randombytes_SEEDBYTES> make_seed()
    {
        std::array<unsigned char, randombytes_SEEDBYTES> seed;
        syscall(SYS_getrandom, seed.data(), randombytes_SEEDBYTES, 1);
        return seed;
    }
    sodium_random()
    {
        if (sodium_init() == -1)
        {
            qmpc::Log::throw_with_trace(
                std::runtime_error("RandomError: There is a serious lack of entropy.")
            );
        };

        syscall(SYS_getrandom, seed.data(), randombytes_SEEDBYTES, 1);
    }
    sodium_random(const std::array<unsigned char, randombytes_SEEDBYTES>& seed) : seed(seed)
    {
        if (sodium_init() == -1)
        {
            qmpc::Log::throw_with_trace(
                std::runtime_error("RandomError: There is a serious lack of entropy.")
            );
        };
    }
    auto generate()
    {
        result_type ret;
        randombytes_buf_deterministic(&ret, sizeof(result_type), seed.data());
        // ret = randombytes_random();
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

private:
    seed_type seed;
};

}  // namespace qmpc::random