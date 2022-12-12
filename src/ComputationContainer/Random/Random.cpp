#include "Random.hpp"

#include <linux/random.h>
#include <sys/syscall.h>
#include <unistd.h>

#include "Random/Csprng.hpp"

unsigned long long RandGenerator::generateRand()
{
    const auto start_tp = std::chrono::system_clock::now();
    if (this->rands.empty())
    {
        Utility::CSPRNG generator = Utility::CSPRNG();
        this->rands = generator.GetRandLLVec(n_hold);
    }

    unsigned long long rnd = static_cast<unsigned long long>(*(this->rands.end() - 1));
    this->rands.pop_back();

    const auto finish_tp = std::chrono::system_clock::now();

    const auto dur =
        std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(finish_tp - start_tp);

    qmpc::RandomGeneratorAcc::generate_rand_acc(dur.count());

    return rnd;
}
