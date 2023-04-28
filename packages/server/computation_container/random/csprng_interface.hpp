#pragma once
#include <sodium.h>
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
    auto operator()()
    {
        std::seed_seq seed;
        return this->operator()(seed);
    }
    auto operator()(const std::seed_seq& seed) { return static_cast<PRNG*>(this)->generate(); }
    auto operator()(size_t n)
    {
        std::seed_seq seed;
        return this->operator()(n, seed);
    }
    /**
     * @brief 乱数ベクターを返す関数
     *
     * @param n サイズ
     * @param seed 乱数生成用シード
     * @return value_type型のvectorを返す関数へのフック
     */
    auto operator()(size_t n, const std::seed_seq& seed)
    {
        return static_cast<PRNG*>(this)->generate(n);
    }
    template <typename T>
    using value_type = typename T::result_type;
    /**
     * @brief 上限と下限を指定する
     *
     * @param min 下限
     * @param max 上限
     * @return 範囲内の乱数生成
     */
    template <typename Result, typename T = PRNG>
    auto clamp(value_type<T> min, value_type<T> max)
    {
        // integral
        if constexpr (std::is_integral_v<Result>)
        {
            std::uniform_int_distribution<Result> dist(min, max);
            return dist(*static_cast<PRNG*>(this));
        }
        // float
        else
        {
            std::uniform_real_distribution<Result> dist(min, max);
            return dist(*static_cast<PRNG*>(this));
        }
    }
};

class sodium_random : public csprng_interface<sodium_random>
{
public:
    sodium_random()
    {
        if (sodium_init() == -1)
        {
            qmpc::Log::throw_with_trace(
                std::runtime_error("RandomError: There is a serious lack of entropy.")
            );
        };
    }
    using result_type = unsigned int;
    auto generate()
    {
        result_type data = randombytes_random();
        return data;
    }
    auto generate(const size_t size, const std::seed_seq& seed) { return 0; }
};

class default_random : public csprng_interface<default_random>
{
public:
    using result_type = unsigned int;
    auto generate() const { return std::random_device()(); }
};

}  // namespace qmpc::random