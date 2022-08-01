#include "Random.hpp"

#include <linux/random.h>
#include <unistd.h>
#include <sys/syscall.h>
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
template <>
std::vector<FixedPoint> RandGenerator::getRandVec<FixedPoint>(
    long long min_val, long long max_val, int n
)
{
    std::vector<FixedPoint> ret;
    ret.reserve(n);
    for (int i = 0; i < n; i++)
    {
        auto rnd = RandGenerator::getInstance()->generateRand();
        auto rnd_mod = static_cast<long long>(rnd % (max_val - min_val));
        ret.emplace_back(FixedPoint(std::abs(rnd_mod) + min_val));
    }
    return ret;
}
template <>
PrimeField RandGenerator::getRand<PrimeField>(long long min_val, long long max_val)
{
    // 1. long long int → unsigned long long (0 <= unsigned_rnd <= 2^{64}-1)
    auto unsigned_rnd = RandGenerator::getInstance()->generateRand();
    unsigned_rnd = (unsigned_rnd % (max_val - min_val) + min_val);
    // 2. unsigned long long → boost::multiprecision::uint128_t
    boost::multiprecision::uint128_t rnd =
        static_cast<boost::multiprecision::uint128_t>(unsigned_rnd);
    PrimeField val(rnd);
    return val;
}
