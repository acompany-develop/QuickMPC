#include "Math.hpp"

#include "Logging/Logger.hpp"
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

    QMPC_LOG_INFO("avg_s: {}", ret.getVal().getStrVal());
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

    QMPC_LOG_INFO("var_s: {}", ret.getVal());
    return ret;
}

FixedPoint stdev(std::vector<Share> &v)
{
    Share var;
    var = variance(v);

    open(var);

    FixedPoint stdev = recons(var);
    QMPC_LOG_INFO("recons_varv: {}", stdev.getStrVal());
    auto value = boost::multiprecision::cpp_dec_float_100(stdev.getStrVal());
    if (value < 0)
    {
        value = 0;
    }
    auto r = sqrt(value);
    QMPC_LOG_INFO("stdev: {}", r);
    FixedPoint ret{r};
    QMPC_LOG_INFO("stdev_ret: {}", ret.getStrVal());
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
    // TODO:一時的に値を入れているので要修正
    if (stdeX <= FixedPoint(0.001) and stdeX >= FixedPoint(-0.001))
    {
        stdeX = FixedPoint("0.001");
    }

    if (stdeY <= FixedPoint(0.001) and stdeY >= FixedPoint(-0.001))
    {
        stdeY = FixedPoint("0.001");
    }

    // if (stdeX.getVal() == 0 or stdeY.getVal() == 0)
    // {
    //     throw std::runtime_error("0 divede error");
    // }
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
Share sigmoid(const Share &x, const FixedPoint &a)
{
    auto exp_ = exp(x * a);
    auto exp_inv = qmpc::Share::getInv(exp_);
    auto exp_1 = exp_inv + 1;
    return qmpc::Share::getInv(exp_1);
}

Share open_sigmoid(const Share &x_s, const FixedPoint &a)
{
    qmpc::Share::open(x_s);
    auto s = qmpc::Share::recons(x_s);
    auto E = FixedPoint(std::exp(-((s * a).getDoubleVal())));
    auto E_s = qmpc::Share::sharize(E);
    auto one_s = qmpc::Share::getConstantShare(FixedPoint("1"));
    auto ret = one_s / (one_s + E_s);
    // 乱数によりまれにinfになることがあるためその場合は計算消し直す
    if (boost::math::isinf(ret.getVal().getVal<double>()))
    {
        ret = qmpc::Math::open_sigmoid(s);
    }
    return ret;
}

Share open_sigmoid_vector(const std::vector<Share> &v_s, const FixedPoint &a)
{
    auto x_s = sum(v_s);
    return open_sigmoid(x_s);
}
}  // namespace qmpc::Math