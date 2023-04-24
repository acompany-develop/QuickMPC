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
    // 突合に使用した列をテーブルから削除する関数
    template <class T>
    static T removeIdColumn(const std::vector<std::list<int>> &arg, const T &v)
    {
        const size_t match_row = arg[0].front() - 1;

        T new_vec;
        new_vec.reserve(v.size());
        for (size_t i = 0; i < v.size(); ++i)
        {
            if (i != match_row)
            {
                new_vec.emplace_back(v[i]);
            }
        }
        return new_vec;
    }

    JoinTableJob(const JobParameter &request) : JobBase<JoinTableJob>(request) {}
    auto compute(
        const std::string job_uuid,
        const qmpc::ComputationToDb::ValueTable &table,
        const std::vector<std::list<int>> &arg
    )
    {
        auto schemas = table.getSchemas();
        auto new_schemas = removeIdColumn(arg, schemas);

        auto db_client = qmpc::ComputationToDb::Client::getInstance();
        auto column_number = new_schemas.size();

        // ID列を削除してTableを保存
        auto computationResultWriterTable =
            qmpc::ComputationToDb::ComputationResultWriter(job_uuid, 1, column_number);
        for (const auto &row : table)
        {
            computationResultWriterTable.emplace(removeIdColumn(arg, row));
        }
        computationResultWriterTable.completed();

        // Schemaを保存
        auto computationResultWriterSchema =
            qmpc::ComputationToDb::ComputationResultWriter(job_uuid, 2, column_number);
        computationResultWriterSchema.emplace(new_schemas);
        computationResultWriterSchema.completed();
    }
};
}  // namespace qmpc::Job
