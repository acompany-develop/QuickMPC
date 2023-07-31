#pragma once
#include <algorithm>
#include <initializer_list>
#include <iostream>
#include <string>
#include <vector>

#include "share/share.hpp"

namespace qmpc::Math
{
using Share = qmpc::Share::Share<FixedPoint>;
Share sum(const std::vector<Share> &v);
Share smean(const std::vector<Share> &v);
Share variance(std::vector<Share> &v);
// 標準偏差
FixedPoint stdev(std::vector<Share> &v);
// 相関係数
Share correl(std::vector<Share> &x, std::vector<Share> &y);
}  // namespace qmpc::Math