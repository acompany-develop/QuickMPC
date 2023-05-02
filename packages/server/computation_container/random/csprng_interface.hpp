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
    std::array<unsigned char, randombytes_SEEDBYTES> seed;

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
    auto generate()
    {
        result_type ret;
        seed = make_seed();
        randombytes_buf_deterministic(&ret, sizeof(result_type), seed.data());
        // ret = randombytes_random();
        return ret;
    }
    auto generate(const size_t size)
    {
        std::vector<result_type> data(size);
        seed = make_seed();
        // TODO: remove
        randombytes_buf_deterministic(data.data(), size * 4, seed.data());

        // TODO: replace this code.
        // randombytes_buf(data.data(), size);

        return data;
    }
    virtual std::array<unsigned char, randombytes_SEEDBYTES> make_seed()
    {
        std::array<unsigned char, randombytes_SEEDBYTES> seed;
        syscall(SYS_getrandom, seed.data(), randombytes_SEEDBYTES, 1);
        return seed;
    }
};

}  // namespace qmpc::random