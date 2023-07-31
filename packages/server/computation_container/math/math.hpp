#pragma once
#include <numeric>
#include <vector>

#include "share/share.hpp"

namespace qmpc::Math
{
using Share = qmpc::Share::Share<FixedPoint>;
Share sum(const std::vector<Share> &v);
Share smean(const std::vector<Share> &v);
std::vector<Share> deviation(std::vector<Share> v);
Share variance(const std::vector<Share> &v);
FixedPoint stdev(const std::vector<Share> &v);
Share covariance(const std::vector<Share> &x, const std::vector<Share> &y);
Share correl(const std::vector<Share> &x, const std::vector<Share> &y);
}  // namespace qmpc::Math