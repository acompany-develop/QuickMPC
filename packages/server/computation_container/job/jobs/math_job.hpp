#pragma once

#include <functional>

#include "client/computation_to_db/client.hpp"
#include "job/job_base.hpp"
#include "math/math.hpp"
#include "share/share.hpp"

namespace qmpc::Job
{
class MathJob : public JobBase<MathJob>
{
    // 数学関数
    using Func = std::function<Share(std::vector<Share> &)>;
    Func f;

    static auto toString(const std::vector<Share> &values)
    {
        std::vector<std::string> results;
        results.reserve(values.size());
        for (const auto &value : values)
        {
            results.emplace_back(value.getVal().getStrVal());
        }
        return results;
    }

public:
    MathJob(Func f, const JobParameter &request) : JobBase<MathJob>(request), f(f) {}
    auto compute(
        const std::string job_uuid,
        const qmpc::ComputationToDb::ValueTable &table,
        const std::vector<std::list<int>> &arg
    )
    {
        std::list<std::vector<Share>> cols;
        for (const auto &w : arg[0])
        {
            std::vector<Share> tmp;
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

        auto results = toString(ret);
        auto computationResultWriter =
            qmpc::ComputationToDb::ComputationResultWriter(job_uuid, 0, results.size());
        computationResultWriter.emplace(results);
        computationResultWriter.write();
    }
};
}  // namespace qmpc::Job
