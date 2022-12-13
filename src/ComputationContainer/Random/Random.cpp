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
