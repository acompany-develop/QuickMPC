#pragma once
#include <chrono>
#include <iostream>
#include <string>
#include <typeinfo>
#include <vector>

#include "math/math.hpp"
#include "nlohmann/json.hpp"
#include "share/share.hpp"

// 計算の状態を管理
extern bool is_computing;

bool compute(
    std::vector<std::string> &values,
    std::vector<std::string> &data_id,
    __int32_t methodId,
    std::vector<std::string> &results
);
void parseData(
    std::vector<std::string> &values, std::vector<std::string> &data_ids, std::vector<Share> &shares
);