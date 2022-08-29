#pragma once
#include <algorithm>
#include <initializer_list>
#include <iostream>
#include <string>
#include <vector>

#include "Share/Share.hpp"

namespace qmpc::Math
{
using Share = qmpc::Share::Share<FixedPoint>;
Share sum(const std::vector<Share> &v);
Share smean(const std::vector<Share> &v);
Share variance(std::vector<Share> &v);
//標準偏差
FixedPoint stdev(std::vector<Share> &v);
//相関係数
Share correl(std::vector<Share> &x, std::vector<Share> &y);
Share exp(const Share &x);
Share sigmoid(const Share &x, const FixedPoint &a = 1);
Share open_sigmoid(const Share &x_s, const FixedPoint &a = 1);
Share open_sigmoid_vector(const std::vector<Share> &v_s, const FixedPoint &a = 1);
}  // namespace qmpc::Math