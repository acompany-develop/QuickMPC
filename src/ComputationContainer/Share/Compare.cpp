
#include "Compare.hpp"

#include "ConfigParse/ConfigParse.hpp"
#include "Logging/Logger.hpp"

namespace qmpc::Share
{

auto convertFpToBool(const FixedPoint &fp, const std::string &op_name)
{
    if (fp.getDoubleVal() > 0.95)
    {
        return true;
    }
    else if (fp.getDoubleVal() >= 0.5)
    {
        QMPC_LOG_ERROR("This operation (%s) determined to be true, but it could be false.", op_name);
        QMPC_LOG_ERROR(
            "If you want to ignore the error and continue the calculation, replace 'exit' with "
            "'return true;'. "
        );
        std::exit(EXIT_FAILURE);
        // return true;
    }
    else if (fp.getDoubleVal() >= 0.05)
    {
        QMPC_LOG_ERROR("This operation (%s) determined to be false, but it could be true.", op_name);
        QMPC_LOG_ERROR(
            "If you want to ignore the error and continue the calculation, replace 'exit' with "
            "'return false;'. "
        );
        std::exit(EXIT_FAILURE);
        // return false;
    }
    else
    {
        return false;
    }
}

// アルゴリズムの詳細はこちら:
// Docs/faster-comparison-operators.md
bool operator<(const Share<FixedPoint> &left, const Share<FixedPoint> &right)
{
    Share<FixedPoint> s = left - right;
    Share s_ltz = LTZ(s);
    open(s_ltz);
    auto ret = recons(s_ltz);
    return convertFpToBool(ret, "Share < Share");
}

// [left == right] <=> [not (left < right)] and [not (right < left)]
// アルゴリズムの詳細はこちら:
// Docs/faster-comparison-operators.md
bool operator==(const Share<FixedPoint> &left, const Share<FixedPoint> &right)
{
    auto x_ret = (left < right);
    auto y_ret = (right < left);
    auto ret = (FixedPoint(1) - x_ret) * (FixedPoint(1) - y_ret);
    return convertFpToBool(ret, "Share == Share");
}

bool operator<(const Share<FixedPoint> &left, const FixedPoint &right)
{
    Share<FixedPoint> s = left - right;
    Share s_ltz = LTZ(s);
    open(s_ltz);
    auto ret = recons(s_ltz);
    return convertFpToBool(ret, "Share < FixedPoint");
}

bool operator==(const Share<FixedPoint> &left, const FixedPoint &right)
{
    auto x_ret = (left < right);
    auto y_ret = (right < left);
    auto ret = (FixedPoint(1) - x_ret) * (FixedPoint(1) - y_ret);
    return convertFpToBool(ret, "Share == FixedPoint");
}

std::vector<bool> allLess(
    const std::vector<Share<FixedPoint>> &left, const std::vector<Share<FixedPoint>> &right
)
{
    auto s = left - right;
    auto s_ltz = LTZ(s);
    open(s_ltz);
    auto fpv = recons(s_ltz);
    std::vector<bool> ret;
    ret.reserve(fpv.size());
    for (const auto &fp : fpv)
    {
        ret.emplace_back(convertFpToBool(fp, "Share < Share"));
    }
    return ret;
}

std::vector<bool> allGreater(
    const std::vector<Share<FixedPoint>> &left, const std::vector<Share<FixedPoint>> &right
)
{
    auto s = right - left;
    auto s_ltz = LTZ(s);
    open(s_ltz);
    auto fpv = recons(s_ltz);
    std::vector<bool> ret;
    ret.reserve(fpv.size());
    for (const auto &fp : fpv)
    {
        ret.emplace_back(convertFpToBool(fp, "Share > Share"));
    }
    return ret;
}

std::vector<bool> allLessEq(
    const std::vector<Share<FixedPoint>> &left, const std::vector<Share<FixedPoint>> &right
)
{
    auto gtv = allGreater(left, right);
    std::vector<bool> ret;
    ret.reserve(gtv.size());
    for (const auto &gt : gtv)
    {
        ret.emplace_back(gt ^ true);
    }
    return ret;
}

std::vector<bool> allGreaterEq(
    const std::vector<Share<FixedPoint>> &left, const std::vector<Share<FixedPoint>> &right
)
{
    auto ltv = allLess(left, right);
    std::vector<bool> ret;
    ret.reserve(ltv.size());
    for (const auto &lt : ltv)
    {
        ret.emplace_back(lt ^ true);
    }
    return ret;
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

// Less Than Zero ([s < 0])
// アルゴリズムの詳細はこちら:
// Docs/faster-comparison-operators.md
Share<FixedPoint> LTZ(const Share<FixedPoint> &s)
{
    // Experimented and adjusted.
    int m = 20;
    int k = 48;

    // s に 2^m をかけて整数化を試みる
    Share<FixedPoint> x = s * FixedPoint(std::to_string(1LL << m));
    Share<FixedPoint> y = FixedPoint(std::to_string(1LL << k)) + x;
    Share<FixedPoint> z = getLSBShare(y);
    y = (y - z) * FixedPoint(0.5);
    for (int i = 1; i < k; ++i)
    {
        Share<FixedPoint> b = getLSBShare(y);
        z += (b * FixedPoint(std::to_string(1LL << i)));
        y = (y - b) * FixedPoint(0.5);
    }
    return (z - x) / FixedPoint(std::to_string(1LL << k));
}

std::vector<Share<FixedPoint>> LTZ(const std::vector<Share<FixedPoint>> &s)
{
    // Experimented and adjusted.
    int m = 20;
    int k = 48;

    auto x = s * FixedPoint(std::to_string(1LL << m));
    auto y = FixedPoint(std::to_string(1LL << k)) + x;
    auto z = getLSBShare(y);
    y = (y - z) * FixedPoint(0.5);
    for (int i = 1; i < k; ++i)
    {
        auto b = getLSBShare(y);
        z = z + (b * FixedPoint(std::to_string(1LL << i)));
        y = (y - b) * FixedPoint(0.5);
    }
    return (z - x) / FixedPoint(std::to_string(1LL << k));
}
}  // namespace qmpc::Share
