#pragma once
#include <sodium.h>
#include <limits>
#include <random>

namespace qmpc::random
{

template <typename PRNG>
struct csprng_interface
{
    std::seed_seq seed;

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
    auto operator()(size_t n, const std::seed_seq& seed)
    {
        return static_cast<PRNG*>(this)->generate(n);
    }
};

class sodium_random : public csprng_interface<sodium_random>
{
public:
    sodium_random() { sodium_init(); }
    using result_type = unsigned long long;
    auto generate()
    {
        result_type data = 1ull * randombytes_random() << 32 + randombytes_random();
        return data;
    }
    auto generate(const size_t byte_size) { return 0; }
};

class default_random : public csprng_interface<default_random>
{
public:
    using result_type = unsigned int;
    auto generate() const { return std::random_device()(); }
};

}  // namespace qmpc::random