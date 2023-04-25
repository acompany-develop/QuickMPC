#pragma once
#include <sodium.h>
#include <limits>
#include <random>

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

    auto operator()() { return static_cast<PRNG*>(this)->gen(); }
};

class sodium_random : public csprng_interface<sodium_random>
{
public:
    using result_type = unsigned int;
    auto gen() { return randombytes_random(); }
};

class default_random : public csprng_interface<default_random>
{
public:
    using result_type = unsigned int;
    inline static constexpr result_type min_value = std::numeric_limits<result_type>::min();
    inline static constexpr result_type max_value = std::numeric_limits<result_type>::max();

    auto gen() { return std::random_device()(); }
};

}  // namespace qmpc::random