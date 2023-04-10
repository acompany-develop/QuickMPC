#include "random.hpp"

#include <linux/random.h>
#include <sys/syscall.h>
#include <unistd.h>

#include "random/csprng.hpp"

std::uint64_t RandGenerator::generateRand()
{
    if (this->rands.empty())
    {
        Utility::CSPRNG generator = Utility::CSPRNG();
        this->rands = generator.GetRandLLVec(n_hold);
    }

    std::uint64_t rnd = static_cast<std::uint64_t>(*(this->rands.end() - 1));
    this->rands.pop_back();

    return rnd;
}
