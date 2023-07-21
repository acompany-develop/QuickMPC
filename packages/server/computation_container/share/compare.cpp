
#include "compare.hpp"

#include "config_parse/config_parse.hpp"
#include "logging/logger.hpp"

using ll = long long;

namespace qmpc::Share
{

Share<ll> right_shift(const Share<ll>&x)
{
    Share<ll> r = getRandBitShare<ll>();
    Share<bool> b((x.getVal() ^ r.getVal()) & 1);

    open(b);
    int sum = recons(b);
    bool c = sum & 1;

    Share<ll> y = x - r + (c ? 2*r : 0) - Share<ll>(b.getVal());
    Config *conf = Config::getInstance();
    if (conf->party_id == conf->sp_id)
    {
        y += ll(sum - c);
    }
    return y / 2;
}

std::vector<Share<ll>> right_shift(const std::vector<Share<ll>>&x)
{
    size_t n=x.size();
    std::vector<Share<ll>> r = getRandBitShare<ll>(n);

    std::vector<Share<bool>> b(n);
    for(int i=0;i<n;i++)
    {
        b[i] = (x[i].getVal() ^ r[i].getVal()) & 1;
    }

    open(b);
    std::vector<int> sum = recons(b);
    std::vector<bool> c(n);
    for(int i=0;i<n;i++)
    {
        c[i] = sum[i] & 1;
    }

    Config *conf = Config::getInstance();
    std::vector<Share<ll>> y(n);
    for(int i=0;i<n;i++)
    {
        y[i] = x[i] - r[i] + (c[i] ? 2*r[i] : 0) - Share<ll>(b[i].getVal());
        if (conf->party_id == conf->sp_id)
        {
            y[i] += ll(sum[i] - c[i]);
        }
        y[i] /= 2;
    }

    return y;
}

bool LTZ(Share<ll> x)
{
    int k = 48;
    x += 1LL << k;
    for(int i=0;i<k;i++)
    {
        x = right_shift(x);
    }
    open(x);
    ll res = recons(x);
    assert(res == 0 || res == 1);
    return res;
}

std::vector<bool> LTZ(std::vector<Share<ll>> x)
{
    int k = 48;
    size_t n = x.size();
    for(int i=0;i<n;i++)
    {
        x[i] += 1LL<<k;
    }

    for(int i=0;i<k;i++)
    {
        x = right_shift(x);
    }
    open(x);
    std::vector<ll> res = recons(x);
    std::vector<bool> b(n);
    for(int i=0;i<n;i++)
    {
        if(!(res[i]==0 || res[i]==1))
        {
            for(int i=0;i<n;i++)std::cerr<<res[i]<<" ";std::cerr<<std::endl;
            assert(false);
        }
        b[i] = res[i]==0;
    }
    return b;
}

std::vector<bool> allLess(
    const std::vector<Share<FixedPoint>> &left, const std::vector<Share<FixedPoint>> &right
)
{
    size_t n = left.size();
    std::vector<Share<ll>> v(n);
    for(int i=0;i<n;i++)
    {
        v[i] = (left[i] - right[i]).getDoubleVal();
    }
    for(int i=0;i<n;i++){
        open(v[i]);
        ll a = recons(v[i]);
        assert((-1LL<<48) <= a && a < (1LL<<48));
    }
    return LTZ(v);
}

std::vector<bool> allGreater(
    const std::vector<Share<FixedPoint>> &left, const std::vector<Share<FixedPoint>> &right
)
{
    return allLess(right, left);
}

std::vector<bool> allLessEq(
    const std::vector<Share<FixedPoint>> &left, const std::vector<Share<FixedPoint>> &right
)
{
    auto res = allGreater(left,right);
    for(int i=0;i<res.size();i++)res[i] = !res[i];
    return res;
}

std::vector<bool> allGreaterEq(
    const std::vector<Share<FixedPoint>> &left, const std::vector<Share<FixedPoint>> &right
)
{
    return allLessEq(right, left);
}

std::vector<bool> allEq(
    const std::vector<Share<FixedPoint>> &left, const std::vector<Share<FixedPoint>> &right
)
{
    auto x_ret = allLess(left, right);
    auto y_ret = allGreater(left, right);
    std::vector<bool> ret;
    ret.reserve(x_ret.size());
    for (size_t i = 0; i < x_ret.size(); ++i)
    {
        ret.emplace_back((x_ret[i] | y_ret[i]) ^ true);
    }
    return ret;
}
}