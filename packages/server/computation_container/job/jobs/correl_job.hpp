#pragma once

#include <list>

#include "client/computation_to_db/client.hpp"
#include "job/job_base.hpp"
#include "math/math.hpp"
#include "share/share.hpp"

namespace qmpc::Job
{
class CorrelJob : public JobBase<CorrelJob>
{
    static auto toString(const std::vector<std::vector<Share>> &values)
    {
        std::vector<std::vector<std::string>> results;
        results.reserve(values.size());
        for (const auto &value : values)
        {
            std::vector<std::string> row;
            row.reserve(values.size());
            for (const auto &x : value)
            {
                row.emplace_back(x.getVal().getStrVal());
            }
            results.emplace_back(row);
        }
        return results;
    }

public:
    using JobBase<CorrelJob>::JobBase;
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
        std::list<std::vector<Share>> targets;
        for (const auto &w : arg[1])
        {
            std::vector<Share> tmp;
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

        auto results = toString(ret);
        auto column_number = (results.empty() ? -1 : results[0].size());
        auto computationResultWriter =
            qmpc::ComputationToDb::ComputationResultWriter(job_uuid, 1, column_number);
        computationResultWriter.emplace(results);
        computationResultWriter.completed();
    }
};
}  // namespace qmpc::Job
