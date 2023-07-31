#include "math.hpp"

#include "logging/logger.hpp"
namespace qmpc::Math
{
Share sum(const std::vector<Share> &v)
{
    Share ret;
    for (const auto &a : v)
    {
        ret += a;
    }
    return ret;
}
Share smean(const std::vector<Share> &v)
{
    // Share avg(FixedPoint(0.0));
    // for (int i = 0; i < size; i++)
    // {
    //     avg = avg + v[i];
    // }
    Share ret{};
    for (auto a : v)
    {
        ret += a;
    }
    int size = std::size(v);
    ret /= FixedPoint(size);

    return ret;
}

Share variance(std::vector<Share> &v)
{
    Share avg;

    avg = smean(v);
    std::vector<Share> var;
    for (auto &a : v)
    {
        var.emplace_back(a - avg);
    }
    auto varVec = var * var;
    Share ret{};
    for (auto &a : varVec)
    {
        ret += a;
    }
    // FPのresolutionの制約により、FPだと1000より大きい数で割れないためdoubleで割っている
    // double var_d = var.getDoubleVal();
    int size = std::size(v);
    ret /= FixedPoint(size);

    return ret;
}

FixedPoint stdev(std::vector<Share> &v)
{
    Share var;
    var = variance(v);

    FixedPoint stdev = open_and_recons(var);
    auto value = stdev.getDoubleVal<qmpc::Utils::mp_float>();
    if (value < 0)
    {
        value = 0;
    }
    auto r = sqrt(value);
    FixedPoint ret{r};
    return ret;
}
Share correl(std::vector<Share> &x, std::vector<Share> &y)
{
    int sizex = (int)x.size();
    int sizey = (int)y.size();

    if (sizex != sizey)
    {
        qmpc::Log::throw_with_trace(std::runtime_error("input Size is not Equal"));
    }
    Share aveX = smean(x);
    Share aveY = smean(y);

    FixedPoint stdeX = stdev(x);
    FixedPoint stdeY = stdev(y);

    // 0徐算エラー
    if (stdeX == FixedPoint(0) || stdeY == FixedPoint(0))
    {
        throw std::runtime_error("correl Div0 error");
    }

    int n = sizex;
    std::vector<Share> tmpX;
    tmpX.reserve(n);
    std::vector<Share> tmpY;
    tmpY.reserve(n);
    for (int i = 0; i < n; ++i)
    {
        tmpX.emplace_back(x[i] - aveX);
        tmpY.emplace_back(y[i] - aveY);
    }
    auto tmpVec = tmpX * tmpY;
    Share ret{};
    for (auto &r : tmpVec)
    {
        ret += r;
    }
    ret /= stdeX;
    ret /= stdeY;
    ret /= FixedPoint(n);
    return ret;
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