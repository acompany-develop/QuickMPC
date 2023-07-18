#pragma once

#include <future>

#include "config_parse/config_parse.hpp"
#include "fixed_point/fixed_point.hpp"
#include "networking.hpp"
#include "share.hpp"
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
}  // namespace qmpc::Share
