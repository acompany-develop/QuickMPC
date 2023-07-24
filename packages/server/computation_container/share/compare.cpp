
#include "compare.hpp"

#include <boost/math/special_functions/fpclassify.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/operators.hpp>
#include "config_parse/config_parse.hpp"
#include "logging/logger.hpp"

namespace qmpc::Share
{
const int bit_length = 48;
using mp_int = boost::multiprecision::cpp_int;

Share<mp_int> right_shift(const Share<mp_int>&x)
{
    Share<mp_int> r = getRandBitShare<mp_int>();
    Share<bool> b(((x.getVal() ^ r.getVal()) & 1) == 1);

    int sum = open_and_recons(b);
    bool c = sum & 1;

    Share<mp_int> y = x - r + (c ? 2*r : Share<mp_int>(0)) - Share<mp_int>(b.getVal());
    Config *conf = Config::getInstance();
    y += mp_int(sum - c);
    assert(y.getVal() % 2 == 0);
    return y / 2;
}

std::vector<Share<mp_int>> right_shift(const std::vector<Share<mp_int>>&x)
{
    size_t n=x.size();
    std::vector<Share<mp_int>> r = getRandBitShare<mp_int>(n);

    std::vector<Share<bool>> b(n);
    for(size_t i=0;i<n;i++)
    {
        b[i] = ((x[i].getVal() ^ r[i].getVal()) & 1) == 1;
    }

    std::vector<int> sum = open_and_recons(b);
    std::vector<bool> c(n);
    for(size_t i=0;i<n;i++)
    {
        c[i] = sum[i] & 1;
    }

    Config *conf = Config::getInstance();
    std::vector<Share<mp_int>> y(n);
    for(size_t i=0;i<n;i++)
    {
        y[i] = x[i] - r[i] + (c[i] ? 2*r[i] : Share<mp_int>(0)) - Share<mp_int>(b[i].getVal());
        y[i] += mp_int(sum[i] - c[i]);
        assert(y[i].getVal() % 2 == 0);
        y[i] /= 2;
    }

    return y;
}

bool LTZ(Share<mp_int> x)
{
    x += 1LL << bit_length;
    for(int i=0;i<bit_length;i++)
    {
        x = right_shift(x);
    }
    Share<FixedPoint> x_fp(x.getVal());
    FixedPoint res = open_and_recons(x_fp);
    assert(res == 0 || res == 1);
    return res == 0;
}

std::vector<bool> LTZ(std::vector<Share<mp_int>> x)
{
    size_t n = x.size();
    for(size_t i=0;i<n;i++)
    {
        x[i] += 1LL << bit_length;
    }

    for(int i=0;i<bit_length;i++)
    {
        x = right_shift(x);
    }
    std::vector<Share<FixedPoint>> x_fp(n);
    for(size_t i=0;i<n;i++)
    {
        x_fp[i] = Share<FixedPoint>(x[i].getVal());
    }
    std::vector<FixedPoint> res = open_and_recons(x_fp);
    std::vector<bool> b(n);
    for(size_t i=0;i<n;i++)
    {
        if(!(res[i]==0 || res[i]==1))
        {
            //for(int i=0;i<n;i++)std::cerr<<res[i]<<" ";std::cerr<<std::endl;
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
    std::vector<Share<mp_int>> v(n);
    for(size_t i=0;i<n;i++)
    {
        v[i] = mp_int((left[i] - right[i]).getVal().getRoundValue());
        //std::cerr<<v[i].getVal()<<" "<<left[i].getVal()<<" "<<right[i].getVal()<<std::endl;
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
    for(size_t i=0;i<res.size();i++)res[i] = !res[i];
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