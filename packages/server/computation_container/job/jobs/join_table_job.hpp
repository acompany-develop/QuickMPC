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

        // argを束縛した関数を生成する
        auto f = std::bind(removeIdColumn<std::vector<std::string>>, arg, std::placeholders::_1);
        // tableの保存
        db_client->writeComputationResult(job_uuid, table, 1, column_number, f);
        // schemaの保存
        db_client->writeComputationResult(job_uuid, new_schemas, 2, column_number);
    }
};
}  // namespace qmpc::Job
