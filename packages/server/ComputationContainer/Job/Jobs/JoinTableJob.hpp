#pragma once
#include <functional>
#include <unordered_set>

#include "Job/JobBase.hpp"
#include "Math/Math.hpp"
#include "Share/Share.hpp"

namespace qmpc::Job
{
class JoinTableJob : public JobBase<JoinTableJob>
{
    static auto removeIdColumn(
        const std::vector<std::vector<std::string>> &table,
        const std::vector<std::string> &schema,
        const std::list<int> &id_column
    )
    {
        // 突合に使用した列をテーブルから削除する
        const size_t match_row = id_column.front() - 1;

        std::vector<std::vector<std::string>> remove_table;
        remove_table.reserve(table.size());
        for (const auto &row : table)
        {
            std::vector<std::string> remove_row;
            remove_row.reserve(row.size());
            for (size_t i = 0; i < row.size(); ++i)
            {
                if (i != match_row)
                {
                    remove_row.emplace_back(row[i]);
                }
            }
            remove_table.emplace_back(remove_row);
        }

        std::vector<std::string> remove_schema;
        remove_schema.reserve(schema.size());
        for (size_t i = 0; i < schema.size(); ++i)
        {
            if (i != match_row)
            {
                remove_schema.emplace_back(schema[i]);
            }
        }

        return std::pair<std::vector<std::vector<std::string>>, std::vector<std::string>>{
            remove_table, remove_schema};
    }

public:
    JoinTableJob(const JobParameter &request) : JobBase<JoinTableJob>(request)
    {
        use_write_db = false;
    }
    nlohmann::json compute(
        const std::vector<std::vector<Share>> &table,
        const std::vector<std::string> &schemas,
        const std::vector<std::list<int>> &arg
    )
    {
        std::vector<std::vector<std::string>> table_string;
        table_string.reserve(table.size());
        for (const auto &row : table)
        {
            std::vector<std::string> row_string;
            row_string.reserve(row.size());
            for (const auto &s : row)
            {
                row_string.emplace_back(s.getVal().getStrVal());
            }
            table_string.emplace_back(row_string);
        }

        auto [remove_id_table, remove_id_schema] = removeIdColumn(table_string, schemas, arg[0]);

        nlohmann::json ret;
        ret["table"] = remove_id_table;
        ret["schema"] = remove_id_schema;
        return ret;
    }
};
}  // namespace qmpc::Job
