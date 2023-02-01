#pragma once

#include <future>

#include "ConfigParse/ConfigParse.hpp"
#include "FixedPoint/FixedPoint.hpp"
#include "Networking.hpp"
#include "Share.hpp"
namespace qmpc::Share
{

bool operator<(const Share<FixedPoint> &left, const Share<FixedPoint> &right);
bool operator==(const Share<FixedPoint> &left, const Share<FixedPoint> &right);
bool operator<(const Share<FixedPoint> &left, const FixedPoint &right);
bool operator==(const Share<FixedPoint> &left, const FixedPoint &right);
std::vector<bool> allLess(
    const std::vector<Share<FixedPoint>> &left, const std::vector<Share<FixedPoint>> &right
);
std::vector<bool> allGreater(
    const std::vector<Share<FixedPoint>> &left, const std::vector<Share<FixedPoint>> &right
);
std::vector<bool> allLessEq(
    const std::vector<Share<FixedPoint>> &left, const std::vector<Share<FixedPoint>> &right
);
std::vector<bool> allGreaterEq(
    const std::vector<Share<FixedPoint>> &left, const std::vector<Share<FixedPoint>> &right
);
std::vector<bool> allEq(
    const std::vector<Share<FixedPoint>> &left, const std::vector<Share<FixedPoint>> &right
);

Share<FixedPoint> LTZ(const Share<FixedPoint> &s);
std::vector<Share<FixedPoint>> LTZ(const std::vector<Share<FixedPoint>> &s);

/// @brief l=32 split array
inline constexpr std::array<int, 7> delta = {5, 5, 5, 5, 5, 5, 2};
template <typename T, std::enable_if_t<std::is_integral_v<T>, std::nullptr_t> = nullptr>
Share<T> operator==(const Share<T> &left, const Share<T> &right)
{
    return equality(left, right);
}
/// @brief generate one-hot-vector used by unitv
/// @tparam N generate share-value mod N
/// @return random share [r] and r-th value is 1 ,otherwise is 0 in n length array
template <size_t N>
std::pair<Share<int>, std::vector<Share<int>>> unitvPrep()
{
    std::vector<Share<int>> e(N * N);
    std::vector<AddressId> addressIds(N * N);
    std::vector<Share<int>> ret(N * N);
    Config *conf = Config::getInstance();
    auto random_s = RandGenerator::getInstance()->getRandVec<long long>(1, 1 << 20, N);
    auto r = RandGenerator::getInstance()->getRand<long long>(0, N - 1);
    int n_parties = conf->n_parties;
    int pt_id = conf->party_id - 1;
    for (size_t i = 0; i < N * N; ++i)
    {
        addressIds[i] = e[i].getId();
    }
    if (pt_id == 0)
    {
        for (size_t i = 0; i < N * N; ++i)
        {
            if (i % N == i / N) e[i] = 1;
            e[i] += random_s[i % N];
        }

        std::rotate(e.begin(), e.begin() + r * N, e.end());
        send(e, (pt_id + 1) % n_parties + 1);
        auto s = receive<int>((pt_id + n_parties - 1) % n_parties + 1, addressIds);

        for (size_t i = 0; i < N * N; ++i)
        {
            ret[i] = s[i] - random_s[i % N];
        }
    }
    else
    {
        auto s = receive<int>((pt_id + n_parties - 1) % n_parties + 1, addressIds);
        for (size_t i = 0; i < N * N; ++i)
        {
            e[i] = s[i] + random_s[i % N];
        }
        std::rotate(e.begin(), e.begin() + r * N, e.end());
        send(e, (pt_id + 1) % n_parties + 1);
        for (size_t i = 0; i < N * N; ++i)
        {
            ret[i] = -random_s[i % N];
        }
    }
    return {r, ret};
}

template <typename T, int N = 32, int N_dash = 32>
std::vector<Share<T>> unitv(const Share<T> &n)
{
    auto [r, v] = unitvPrep<32>();
    auto diff = n - r;
    open(diff);
    auto rec = recons(diff);
    int m = (rec % N + N) % N;
    std::vector<Share<T>> ret(N);
    for (int i = 0; i < N; ++i)
    {
        ret[i] = v[N * m + i];
    }
    return ret;
}
template <typename Arr>
std::vector<int> expand(int x, const Arr &delta)
{
    unsigned int x_u = static_cast<unsigned int>(x);
    std::vector<int> delta_sum;
    std::vector<int> ret;
    delta_sum.emplace_back(0);
    int sum = 0;
    for (auto &&a : delta)
    {
        sum += a;
        delta_sum.emplace_back(sum);
    }

    reverse(delta_sum.begin(), delta_sum.end());
    for (auto &&delta_num : delta_sum)
    {
        ret.emplace_back(static_cast<int>(1ll * x_u / (1ll << delta_num)));
        x_u = static_cast<unsigned int>(1ll * x_u % (1ll << delta_num));
    }
    return ret;
}
template <typename T>
Share<T> equality1(const Share<T> &x, const Share<T> &y)
{
    auto g = unitv<T>(x - y);
    return g[0];
}
template <typename T>
Share<T> equality(const Share<T> &x, const Share<T> &y)
{
    Config *conf = Config::getInstance();
    int d{};
    if (conf->party_id == 1)
    {
        d = x.getVal() - y.getVal();
    }
    else
    {
        d = y.getVal() - x.getVal();
    }
    auto d_expand = expand(d, delta);
    int m = d_expand.size();
    std::vector<Share<int>> p(m), q(m);
    Share<T> f{};
    for (int i = 0; i < m; ++i)
    {
        if (conf->party_id == 1)
        {
            p[i] = d_expand[i];
        }
        else
        {
            q[i] = d_expand[i];
        }
        f += equality1(p[i], q[i]);
    }
    return unitv(f)[m];
}
}  // namespace qmpc::Share
