#pragma once

#include <future>

#include "ConfigParse/ConfigParse.hpp"
#include "FixedPoint/FixedPoint.hpp"
#include "Networking.hpp"
#include "OT/OT.hpp"
#include "Share.hpp"
namespace qmpc::Share
{

namespace bm = boost::multiprecision;
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

template <typename T, std::enable_if_t<std::is_integral_v<T>, std::nullptr_t> = nullptr>
Share<T> operator==(const Share<T> &left, const Share<T> &right)
{
    return equality(left, right);
}
/// @brief generate one-hot-vector used by unitv
/// @tparam N generate share-value mod N
/// @return random share [r] and r-th value is 1 ,otherwise is 0 in n length array
template <size_t N>
std::pair<Share<bm::cpp_int>, std::vector<Share<bm::cpp_int>>> unitvPrep()
{
    std::vector<qmpc::OT> ot(3, qmpc::OT(N));
    Config *conf = Config::getInstance();
    int pt_id = conf->party_id;
    int n_parties = conf->n_parties;
    int r = RandGenerator::getInstance()->getRand<long long>(1, N);
    auto random_s = RandGenerator::getInstance()->getRandVec<long long>(1, 1 << N - 1, N - 1);
    qmpc::Share::Share<int> rShare = r;
    std::vector<qmpc::Share::Share<bm::cpp_int>> ret(N);
    std::vector<std::vector<bm::cpp_int>> v(n_parties, std::vector<bm::cpp_int>(N));
    if (pt_id == 1)
    {
        // v[to] = data;
        std::vector<bm::cpp_int> e(N);
        e[(r + N - 1) % N] = 1ll;
        v[0] = e;
    }
    for (int party = 1; party <= n_parties; ++party)
    {
        std::vector<std::vector<bm::cpp_int>> x(N, std::vector<bm::cpp_int>(N));
        if (party < pt_id)
        {
            // v[to] = data;
            auto rec = ot[party - 1].recieve(party, r);
            for (int i = 0; i < N; ++i)
            {
                v[party - 1][i] = rec[i];
                // std::cout << "party 2 rec is " << v[1][i] << std::endl;
                v[pt_id - 1][i] += rec[i];
            }
        }
        else if (party > pt_id)
        {
            auto ri = RandGenerator::getInstance()->getRandVec<long long>(1, (1 << N) - 1, N);

            for (int j = 0; j < N; ++j)
            {
                v[party - 1][j] = bm::cpp_int{ri[j]};
            }
            for (int i = 0; i < N; ++i)
            {
                auto vi = v[party - 2];
                std::rotate(vi.begin(), vi.begin() + N - i - 1, vi.end());
                std::vector<bm::cpp_int> xi(N);
                for (int j = 0; j < N; ++j)
                {
                    xi[j] = vi[j] - v[party - 1][j];
                }
                x[i] = xi;
            }
            ot[pt_id - 1].send(party, x);
        }
    }
    for (int i = 0; i < N; ++i)
    {
        ret[i] = v[n_parties - 1][i];
    }
    return {bm::cpp_int{r}, ret};
}
// template <size_t N, size_t ArraySize>
// std::pair<std::vector<Share<bm::cpp_int>>, std::vector<std::vector<Share<bm::cpp_int>>>>
// unitvPrepVec()
// {
//     qmpc::OTe ot(N, ArraySize);
//     Config *conf = Config::getInstance();
//     int pt_id = conf->party_id;
//     int n_parties = conf->n_parties;
//     auto r = RandGenerator::getInstance()->getRandVec<long long>(1, N, ArraySize);
//     std::vector<std::vector<qmpc::Share::Share<bm::cpp_int>>> ret(
//         ArraySize, std::vector<std::vector<qmpc::Share::Share<bm::cpp_int>>>(N)
//     );
//     std::vector<std::vector<std::vector<bm::cpp_int>>> v(
//         ArraySize, std::vector<std::vector<bm::cpp_int>>(n_parties, std::vector<bm::cpp_int>(N))
//     );
//     if (pt_id == 1)
//     {
//         // v[to] = data;
//         std::vector<bm::cpp_int> e(N);
//         e[(r + N - 1) % N] = 1ll;
//         for (int i = 0; i < ArraySize; ++i)
//         {
//             v[i][0] = e;
//         }
//     }
//     for (int party = 1; party <= n_parties; ++party)
//     {
//         std::vector<std::vector<std::vector<bm::cpp_int>>> x(
//             ArraySize, std::vector < std::vector<bm::cpp_int>(N, std::vector<bm::cpp_int>(N))
//         );
//         if (party < pt_id)
//         {
//             // v[to] = data;
//             auto rec = ot.recieve(party, r);
//             for (int i = 0; i < N; ++i)
//             {
//                 v[party - 1][i] = rec[i];
//                 // std::cout << "party 2 rec is " << v[1][i] << std::endl;
//                 v[pt_id - 1][i] += rec[i];
//             }
//         }
//         else if (party > pt_id)
//         {
//             auto ri = RandGenerator::getInstance()->getRandVec<long long>(1, (1 << N) - 1, N);

//             for (int j = 0; j < N; ++j)
//             {
//                 v[party - 1][j] = bm::cpp_int{ri[j]};
//             }
//             for (int i = 0; i < N; ++i)
//             {
//                 auto vi = v[party - 2];
//                 std::rotate(vi.begin(), vi.begin() + N - i - 1, vi.end());
//                 std::vector<bm::cpp_int> xi(N);
//                 for (int j = 0; j < N; ++j)
//                 {
//                     xi[j] = vi[j] - v[party - 1][j];
//                 }
//                 x[i] = xi;
//             }
//             ot.send(party, x);
//         }
//     }
//     for (int i = 0; i < N; ++i)
//     {
//         ret[i] = v[n_parties - 1][i];
//     }
//     return {bm::cpp_int{r}, ret};
// }
template <typename T, int N = 32, int N_dash = 32>
std::vector<Share<T>> unitv(const Share<T> &n)
{
    auto [r, v] = unitvPrep<32>();
    auto diff = n - r;
    open(diff);
    auto rec = recons(diff);
    int m = (rec % N + N) % N;
    // std::vector<Share<T>> ret(N);
    // for (int i = 0; i < N; ++i)
    // {
    //     ret[i] = v[N * m + i];
    // }
    std::rotate(v.begin(), v.begin() + N - m - 1, v.end());
    return v;
}
template <typename T, int N = 32, int N_dash = 32>
std::vector<std::vector<Share<T>>> unitv(const std::vector<Share<T>> &n)
{
    auto [r, v] = unitvPrep<32>();
    auto diff = n - r;
    open(diff);
    auto rec = recons(diff);
    auto m = (rec % N + N) % N;
    // std::vector<Share<T>> ret(N);
    // for (int i = 0; i < N; ++i)
    // {
    //     ret[i] = v[N * m + i];
    // }
    std::rotate(v.begin(), v.begin() + N - m - 1, v.end());
    return v;
}
template <typename T, size_t N = 32>
std::vector<T> expand(const T &x)
{
    T x_dash = x;
    constexpr int mask_size = 8;
    constexpr int size = N / mask_size;
    std::vector<T> ret;
    ret.reserve(size);
    for (int i = 0; i < size; ++i)
    {
        int mask = (1 << mask_size) - 1;
        int x_mask = x_dash & mask;
        // std::cout << "mask_size is " << mask_size << " x_dash is " << std::bitset<16>(x_mask)
        //           << std::endl;
        ret.emplace_back(x_mask);
        x_dash >>= mask_size;
        // std::cout << std::bitset<N>(x_dash) << std::endl;
    }
    std::reverse(ret.begin(), ret.end());
    return ret;
}
template <typename T>
Share<T> equality1(const Share<T> &x, const Share<T> &y)
{
    auto g = unitv<T>(x - y);
    return g[0];
}
template <typename T>
std::vector<Share<T>> equality1(const std::vector<Share<T>> &x, const std::vector<Share<T>> &y)
{
    auto g = unitv<T>(x - y);
    return g[0];
}
template <typename T>
Share<T> equality(const Share<T> &x, const Share<T> &y)
{
    Config *conf = Config::getInstance();
    T d{};
    if (conf->party_id == 1)
    {
        d = x.getVal() - y.getVal();
    }
    else
    {
        d = y.getVal() - x.getVal();
    }
    auto d_expand = expand(d);
    int m = d_expand.size();
    std::vector<Share<T>> p(m), q(m);
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
// template <typename T>
// std::vector<Share<T>> equality(const std::vector<Share<T>> &x, const std::vector<Share<T>> &y)
// {
//     Config *conf = Config::getInstance();
//     int size = x.size();
//     std::vector<T> d(size);
//     if (conf->party_id == 1)
//     {
//         d = x - y;
//     }
//     else
//     {
//         d = y - x;
//     }
//     auto d_expand = expand(d);
//     int m = d_expand[0].size();
//     std::vector<std::vector<T>> p(m, std::vector<T>(size)), q(m, std::vector<T>(size));
//     std::vector<T> f(size);
//     for (int i = 0; i < m; ++i)
//     {
//         if (conf->party_id == 1)
//         {
//             p[i] = d_expand[i];
//         }
//         else
//         {
//             q[i] = d_expand[i];
//         }
//         f = f + equality1(p[i], q[i]);
//     }
//     return unitv(f)[m];
// }
}  // namespace qmpc::Share
