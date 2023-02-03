#pragma once

#include <list>

#include "job/JobBase.hpp"
#include "math/math.hpp"
#include "share/share.hpp"

namespace qmpc::Job
{
class CorrelJob : public JobBase<CorrelJob>
{
public:
    using JobBase<CorrelJob>::JobBase;
    std::vector<std::vector<Share>> compute(
        const std::vector<std::vector<Share>> &table,
        const std::vector<std::string> &schemas,
        const std::vector<std::list<int>> &arg
    )
    {
        std::list<std::vector<Share>> cols;
        for (const auto &w : arg[0])
        {
            std::vector<Share> tmp;
            tmp.reserve(table.size());
            for (const auto &row : table)
            {
                tmp.emplace_back(row[w - 1]);
            }
            cols.emplace_back(tmp);
        }
        std::list<std::vector<Share>> targets;
        for (const auto &w : arg[1])
        {
            std::vector<Share> tmp;
            tmp.reserve(table.size());
            for (const auto &row : table)
            {
                tmp.emplace_back(row[w - 1]);
            }
            targets.emplace_back(tmp);
        }

        std::vector<std::vector<Share>> ret;
        ret.reserve(arg[1].size());
        for (auto &&target : targets)
        {
            std::vector<Share> ret_row;
            ret_row.reserve(arg[0].size());
            for (auto &&col : cols)
            {
                ret_row.emplace_back(qmpc::Math::correl(col, target));
            }
            ret.emplace_back(ret_row);
        }
        return ret;
    }
};
}  // namespace qmpc::Job
