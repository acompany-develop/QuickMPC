#pragma once
#include <chrono>
#include <iostream>
#include <string>
#include <typeinfo>
#include <vector>

#include "Math/Math.hpp"
#include "Share/Share.hpp"
#include "nlohmann/json.hpp"

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