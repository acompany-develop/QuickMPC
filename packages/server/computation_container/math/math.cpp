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

Share exp(const Share &x)
{
    // Nはマクローリン展開時の項数
    // 1+x+x^2 ...  x^N-1
    constexpr int N = 100;
    auto *conf = Config::getInstance();
    Share ret;
    if (conf->sp_id == conf->party_id)
    {
        ret += 1;
    }
    std::vector<Share> px(N);
    std::vector<FixedPoint> k(N);
    px[0] = ret;
    k[0] = 1;
    for (int i = 1; i < N; ++i)
    {
        k[i] = k[i - 1] * i;
        px[i] = px[i - 1] * x;
    }
    for (int i = 1; i < N; ++i)
    {
        px[i] /= k[i];
    }
    return std::accumulate(px.begin(), px.end(), Share{0});
}
}  // namespace qmpc::Math