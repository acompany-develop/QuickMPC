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
     * @return value_type型のvectorを返す関数へのフック
     */
    auto operator()(size_t n) { return static_cast<PRNG*>(this)->generate(n); }
};

class default_seed : public csprng_interface<default_seed>
{
public:
    using result_type = unsigned char;
    auto generate(size_t n)
    {
        std::vector<unsigned char> seed(n);
        syscall(SYS_getrandom, seed.data(), n, 1);
        return seed;
    }
};
template <typename SEED_TYPE = default_seed>
class sodium_random : public csprng_interface<sodium_random<SEED_TYPE>>
{
public:
    using result_type = unsigned int;
    sodium_random()
    {
        if (sodium_init() == -1)
        {
            qmpc::Log::throw_with_trace(
                std::runtime_error("RandomError: There is a serious lack of entropy.")
            );
        };
    }

    auto generate(size_t size)
    {
        std::vector<result_type> data(size);
        auto seed = seed_generator(randombytes_SEEDBYTES);
        randombytes_buf_deterministic(data.data(), sizeof(result_type) * size, seed.data());
        return data;
    }
    auto generate() { return generate(1)[0]; }

private:
    SEED_TYPE seed_generator;
};

}  // namespace qmpc::random