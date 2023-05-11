#pragma once

#include <vector>

#include "csprng_interface.hpp"
#include "fixed_point/fixed_point.hpp"

class RandGenerator
{
private:
    size_t n_hold = 1000;
    std::vector<std::int64_t> rands;

    RandGenerator(){};
    ~RandGenerator(){};

    std::uint64_t generateRand();

public:
    static RandGenerator *getInstance()
    {
        static RandGenerator obj;
        return &obj;
    };

    template <class T>
    T getRand(std::int64_t min_val = 0, std::int64_t max_val = 9223372036854775807ll)
    {
        auto rnd = RandGenerator::getInstance()->generateRand();
        auto rnd_mod = static_cast<std::int64_t>(rnd % (max_val - min_val));
        auto val = T(std::abs(rnd_mod) + min_val);
        return val;
    }

    template <class T>
    std::vector<T> getRandVec(
        std::int64_t min_val = 0, std::int64_t max_val = 9223372036854775807ll, int n = 5
    )
    {
        std::vector<T> ret;
        ret.reserve(n);
        for (int i = 0; i < n; i++)
        {
            ret.emplace_back(getRand<T>(min_val, max_val));
        }
        return ret;
    }
};

template <typename Random>
struct random_interface
{
public:
};
template <typename CSPRNG>
class random_csprng
{
    CSPRNG prng;

public:
    random_csprng() : prng() {}
    /**
     * @brief 上限と下限を指定する
     *
     * @param min 下限
     * @param max 上限
     * @return 範囲内の乱数生成
     */
    template <typename Result>
    auto get(
        Result min = std::numeric_limits<Result>::min(),
        Result max = std::numeric_limits<Result>::max()
    )
    {
        // integral
        if constexpr (std::is_integral_v<Result>)
        {
            std::uniform_int_distribution<Result> dist(min, max);
            return dist(prng);
        }
        // float
        else
        {
            std::uniform_real_distribution<Result> dist(min, max);
            return dist(prng);
        }
    }

    template <typename Result>
    auto get_array(
        size_t size,
        Result min = std::numeric_limits<Result>::min(),
        Result max = std::numeric_limits<Result>::max()
    )
    {
        std::vector<Result> ret(size);
        for (auto &a : ret)
        {
            a = this->get<Result>(min, max);
        }
        return ret;
    }
};