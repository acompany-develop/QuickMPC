#pragma once
#include <functional>
#include <unordered_set>

#include "client/computation_to_db/client.hpp"
#include "job/job_base.hpp"
#include "math/math.hpp"
#include "share/share.hpp"

namespace qmpc::Job
{
class JoinTableJob : public JobBase<JoinTableJob>
{
public:
    JoinTableJob(const JobParameter &request) : JobBase<JoinTableJob>(request) {}
    auto compute(
        const std::string job_uuid,
        const qmpc::ComputationToDb::ValueTable &table,
        const std::vector<std::list<int>> &arg
    )
    {
        // 突合に使用した列をテーブルから削除する関数
        auto removeIdColumn = [&arg](const std::vector<std::string> &v)
        {
            const size_t match_row = arg[0].front() - 1;

            std::vector<std::string> new_vec;
            new_vec.reserve(v.size());
            for (size_t i = 0; i < v.size(); ++i)
            {
                if (i != match_row)
                {
                    new_vec.emplace_back(v[i]);
                }
            }
            return new_vec;
        };

        auto schemas = table.getSchemas();
        auto new_schemas = [&arg](const std::vector<qmpc::ComputationToDb::SchemaType> &v)
        {
            const size_t match_row = arg[0].front() - 1;

            std::vector<qmpc::ComputationToDb::SchemaType> new_vec;
            new_vec.reserve(v.size());
            for (size_t i = 0; i < v.size(); ++i)
            {
                if (i != match_row)
                {
                    new_vec.emplace_back(v[i]);
                }
            }
            return new_vec;
        }(schemas);

        auto db_client = qmpc::ComputationToDb::Client::getInstance();
        auto column_number = new_schemas.size();
        // tableの保存
        db_client->writeComputationResult(job_uuid, table, 1, column_number, removeIdColumn);
        // schemaの保存
        db_client->writeComputationResult(job_uuid, new_schemas, 2, column_number);
    }
};
}  // namespace qmpc::Job
