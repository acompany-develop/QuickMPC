#pragma once

#include <functional>

#include "job/job_base.hpp"
#include "math/math.hpp"
#include "share/share.hpp"

namespace qmpc::Job
{
class MathJob : public JobBase<MathJob>
{
    //数学関数
    using Func = std::function<Share(std::vector<Share> &)>;
    Func f;

public:
    MathJob(Func f, const JobParameter &request) : JobBase<MathJob>(request), f(f) {}
    std::vector<Share> compute(
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
        std::vector<Share> ret;
        ret.reserve(arg[0].size());
        for (auto &&col : cols)
        {
            ret.emplace_back(f(col));
        }
        return ret;
    }
};
}  // namespace qmpc::Job
