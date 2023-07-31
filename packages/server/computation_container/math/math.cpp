#include "math.hpp"

#include "logging/logger.hpp"
namespace qmpc::Math
{
Share sum(const std::vector<Share> &v)
{
    Share e(FixedPoint(0));
    return std::accumulate(v.begin(), v.end(), e);
}
Share smean(const std::vector<Share> &v) { return sum(v) / FixedPoint(std::size(v)); }

std::vector<Share> deviation(std::vector<Share> v)
{
    Share avg = smean(v);
    for (auto &s : v)
    {
        s -= avg;
    }
    return v;
}

Share variance(const std::vector<Share> &v)
{
    auto dev = deviation(v);
    auto var = dev * dev;
    return smean(var);
}

FixedPoint stdev(const std::vector<Share> &v)
{
    Share var = variance(v);
    FixedPoint var_val = open_and_recons(var);

    auto value = var_val.getDoubleVal<qmpc::Utils::mp_float>();
    if (value < 0)
    {
        value = 0;
    }
    return FixedPoint(sqrt(value));
}

Share covariance(const std::vector<Share> &x, const std::vector<Share> &y)
{
    auto devX = deviation(x);
    auto devY = deviation(y);
    auto devXY = devX * devY;
    return smean(devXY);
}

Share correl(const std::vector<Share> &x, const std::vector<Share> &y)
{
    size_t n = x.size();

    if (n != y.size())
    {
        qmpc::Log::throw_with_trace(std::runtime_error("input Size is not Equal"));
    }

    FixedPoint stdeX = stdev(x);
    FixedPoint stdeY = stdev(y);

    // 0除算
    if (stdeX == FixedPoint(0) || stdeY == FixedPoint(0))
    {
        qmpc::Log::throw_with_trace(std::runtime_error("correl Div0 error"));
    }

    return covariance(x, y) / (stdeX * stdeY);
}
}  // namespace qmpc::Math