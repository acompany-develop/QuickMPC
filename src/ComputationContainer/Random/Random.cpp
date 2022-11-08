#include "Random.hpp"

#include <linux/random.h>
#include <sys/syscall.h>
#include <unistd.h>

#include "Random/Csprng.hpp"

unsigned long long RandGenerator::generateRand()
{
    if (this->rands.empty())
    {
        Utility::CSPRNG generator = Utility::CSPRNG();
        this->rands = generator.GetRandLLVec(n_hold);
    }

    unsigned long long rnd = static_cast<unsigned long long>(*(this->rands.end() - 1));
    this->rands.pop_back();

    return rnd;
}

template <>
long long RandGenerator::getRand<long long>(long long min_val, long long max_val)
{
    auto rnd = RandGenerator::getInstance()->generateRand();
    auto rnd_mod = static_cast<long long>(rnd % (max_val - min_val));
    auto val = std::abs(rnd_mod) + min_val;
    return val;
}
template <>
FixedPoint RandGenerator::getRand<FixedPoint>(long long min_val, long long max_val)
{
    auto rnd = RandGenerator::getInstance()->generateRand();
    auto rnd_mod = static_cast<long long>(rnd % (max_val - min_val));
    FixedPoint val{std::abs(rnd_mod) + min_val};
    return val;
}
