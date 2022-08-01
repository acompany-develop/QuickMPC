#pragma once

#include "Model/ModelBase.hpp"
#include "Client/ComputationToDbGate/Client.hpp"
#include "ConfigParse/ConfigParse.hpp"
#include "GBDT/SID3.hpp"

namespace qmpc::Model
{
class SID3Model : public ModelBase
{
    auto parseTable(const std::vector<std::vector<Share>> &table, const std::list<int> &arg) const
    {
        int sample_size = table.size();
        /**
         * | x_11 ... x_1d |
         * | .  .       .  |
         * | x_s1 ... x_sd |
         */
        int dim_x = arg.size();
        std::vector<std::vector<Share>> x;
        x.reserve(sample_size);
        for (const auto &row : table)
        {
            std::vector<Share> tmp;
            tmp.reserve(dim_x);
            for (const auto &j : arg)
            {
                tmp.emplace_back(row[j - 1]);
            }
            x.emplace_back(tmp);
        }
        return x;
    }
    auto parseSchema(const std::vector<std::string> &schema, const std::list<int> &arg) const
    {
        std::vector<std::string> parsed_schema;
        parsed_schema.reserve(arg.size());
        for (const auto &j : arg)
        {
            parsed_schema.emplace_back(schema[j - 1]);
        }
        return parsed_schema;
    }
    auto getAttrSizeList(const std::vector<std::string> &schemas) const
    {
        std::deque<int> attr_size_list;
        int size = 0;
        for (const auto &s : schemas)
        {
            for (auto c = s.rbegin(); c != s.rend(); ++c)
            {
                if (*c == '#')
                {
                    if (size > 0 && *std::prev(c) == '0')
                    {
                        attr_size_list.emplace_back(size);
                        size = 0;
                    }
                    break;
                }
            }
            ++size;
        }
        attr_size_list.emplace_back(size);
        return attr_size_list;
    }

    auto getBitvector(
        const std::vector<std::vector<Share>> &table, const std::deque<int> &attr_size_list
    ) const
    {
        std::vector<std::vector<std::vector<Share>>> data;
        data.reserve(table.size());
        for (const auto &row : table)
        {
            std::vector<std::vector<Share>> tmp_row;
            tmp_row.reserve(attr_size_list.size());

            int from = 0;
            for (const auto &size : attr_size_list)
            {
                int to = from + size;
                std::vector<Share> tmp;
                tmp.reserve(size);
                for (int i = from; i < to; ++i)
                {
                    tmp.emplace_back(row[i]);
                }
                tmp_row.emplace_back(tmp);
                from += size;
            }
            data.emplace_back(tmp_row);
        }
        return data;
    }

public:
    std::vector<Share> predict(
        const std::vector<std::vector<Share>> &table,
        const std::vector<std::string> &schemas,
        const std::list<int> &arg,
        const std::string &model_param_job_uuid
    ) const override
    {
        auto db_client = qmpc::ComputationToDbGate::Client::getInstance();
        auto sid3_param_json = db_client->readModelparamJson(model_param_job_uuid);
        auto sid3_tree = std::make_unique<qmpc::GBDT::SID3>(sid3_param_json);

        auto x = parseTable(table, arg);
        auto attr_size_list = getAttrSizeList(parseSchema(schemas, arg));
        auto data = getBitvector(x, attr_size_list);

        std::vector<Share> result;
        result.reserve(x.size());
        for (const auto &d : data)
        {
            result.emplace_back(sid3_tree->predict(d));
        }
        return result;
    }
};
}  // namespace qmpc::Model