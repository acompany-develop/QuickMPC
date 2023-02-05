#pragma once
#include <vector>

#include "share/share.hpp"

namespace qmpc::ObjectiveFunction
{
/*
目的関数
入力
    予測値y
    目的値t
出力関数
    関数を持っている場合は間数f
    持っていない場合でも勾配df（導関数を持っていない場合はepsで微分する）
*/
class ObjectiveFunctionInterface
{
    using Share = ::Share;

public:
    //全データの勾配を取得する
    virtual std::vector<Share> df(const std::vector<Share> &) const = 0;
    // sgdで使用する
    // num個のデータでの勾配を取得する
    virtual std::vector<Share> df(int num, const std::vector<Share> &) const = 0;
};
}  // namespace qmpc::ObjectiveFunction