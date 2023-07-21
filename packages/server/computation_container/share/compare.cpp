
#include "compare.hpp"

#include "config_parse/config_parse.hpp"
#include "logging/logger.hpp"

using lint = long long;
const int bit_length = 50;

namespace qmpc::Share
{

Share<lint> right_shift(const Share<lint>&x)
{
    Share<lint> r = getRandBitShare<lint>();
    Share<bool> b((x.getVal() ^ r.getVal()) & 1);

    open(b);
    int sum = recons(b);
    bool c = sum & 1;

    Share<lint> y = x - r + (c ? 2*r : Share<lint>(0)) - Share<lint>(b.getVal());
    Config *conf = Config::getInstance();
    if (conf->party_id == conf->sp_id)
    {
        y += lint(sum - c);
    }
    assert(y.getVal() % 2 == 0);
    return y / 2;
}

std::vector<Share<lint>> right_shift(const std::vector<Share<lint>>&x)
{
    size_t n=x.size();
    std::vector<Share<lint>> r = getRandBitShare<lint>(n);

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
    std::vector<Share<lint>> y(n);
    for(int i=0;i<n;i++)
    {
        y[i] = x[i] - r[i] + (c[i] ? 2*r[i] : Share<lint>(0)) - Share<lint>(b[i].getVal());
        if (conf->party_id == conf->sp_id)
        {
            y[i] += lint(sum[i] - c[i]);
        }
        assert(y[i].getVal() % 2 == 0);
        y[i] /= 2;
    }

    return y;
}

bool LTZ(Share<lint> x)
{
    x += 1LL << bit_length;
    for(int i=0;i<bit_length;i++)
    {
        x = right_shift(x);
    }
    open(x);
    lint res = recons(x);
    assert(res == 0 || res == 1);
    return res;
}

std::vector<bool> LTZ(std::vector<Share<lint>> x)
{
    size_t n = x.size();
    for(int i=0;i<n;i++)
    {
        x[i] += 1LL << bit_length;
    }

    for(int i=0;i<bit_length;i++)
    {
        x = right_shift(x);
    }
    open(x);
    std::vector<lint> res = recons(x);
    std::vector<bool> b(n);
    for(int i=0;i<n;i++)
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
    std::vector<Share<lint>> v(n);
    for(int i=0;i<n;i++)
    {
        v[i] = lint((left[i] - right[i]).getVal().getRoundValue());
        //std::cerr<<v[i].getVal()<<" "<<left[i].getVal()<<" "<<right[i].getVal()<<std::endl;
    }
    for(int i=0;i<n;i++){
        open(v[i]);
        lint a = recons(v[i]);
        assert((-1LL<<bit_length) <= a && a < (1LL<<bit_length));
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