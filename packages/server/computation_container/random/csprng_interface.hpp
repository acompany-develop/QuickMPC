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

class sodium_seed
{
public:
    using seed_type = std::array<unsigned char, randombytes_SEEDBYTES>;
    sodium_seed() : seed() { syscall(SYS_getrandom, seed.data(), randombytes_SEEDBYTES, 1); }
    sodium_seed(const seed_type& const_seed) : seed(const_seed) {}
    seed_type operator()() { return seed; }

private:
    seed_type seed;
};
template <typename SEED>
class sodium_random
{
    SEED seed;

public:
    using result_type = unsigned int;
    sodium_random() : seed()
    {
        if (sodium_init() == -1)
        {
            qmpc::Log::throw_with_trace(
                std::runtime_error("RandomError: There is a serious lack of entropy.")
            );
        };
    }
    sodium_random(const SEED& seed) : seed(seed) {}
    auto operator()() { return operator()(1)[0]; }
    auto operator()(size_t size)
    {
        std::vector<result_type> data(size);
        randombytes_buf_deterministic(data.data(), sizeof(result_type) * size, seed().data());
        return data;
    }
};

}  // namespace qmpc::random