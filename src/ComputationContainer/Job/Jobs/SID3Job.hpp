#pragma once

#include <list>

#include "Job/JobBase.hpp"
#include "ConfigParse/ConfigParse.hpp"
#include "Share/Matrix.hpp"
#include "GBDT/SID3.hpp"

namespace qmpc::Job
{
class SID3Job : public JobBase<SID3Job>
{
    using JobBase<SID3Job>::JobBase;

    auto parse_table(
        const std::vector<std::vector<Share>> &table, const std::vector<std::list<int>> &arg
    ) const
    {
        int sample_size = table.size();
        /**
         * | x_11 ... x_1d |
         * | .  .       .  |
         * | x_s1 ... x_sd |
         */
        int dim_x = arg[0].size();
        std::vector<std::vector<Share>> x;
        x.reserve(sample_size);
        for (const auto &row : table)
        {
            std::vector<Share> tmp;
            tmp.reserve(dim_x);
            for (const auto &j : arg[0])
            {
                tmp.emplace_back(row[j - 1]);
            }
            x.emplace_back(tmp);
        }

        /**
         * | y_11 ... y_1d |
         * | .  .       .  |
         * | y_s1 ... y_sd |
         */
        int dim_y = arg[1].size();
        std::vector<std::vector<Share>> y;
        y.reserve(sample_size);
        for (const auto &row : table)
        {
            std::vector<Share> tmp;
            tmp.reserve(dim_y);
            for (const auto &j : arg[1])
            {
                tmp.emplace_back(row[j - 1]);
            }
            y.emplace_back(tmp);
        }

        return std::pair<std::vector<std::vector<Share>>, std::vector<std::vector<Share>>>{x, y};
    }
    auto parse_schema(
        const std::vector<std::string> &schemas, const std::vector<std::list<int>> &arg
    ) const
    {
        std::vector<std::string> schemaX;
        schemaX.reserve(arg[0].size());
        for (const auto &j : arg[0])
        {
            schemaX.emplace_back(schemas[j - 1]);
        }

        std::vector<std::string> schemaY;
        schemaY.reserve(arg[1].size());
        for (const auto &j : arg[1])
        {
            schemaY.emplace_back(schemas[j - 1]);
        }
        return std::pair<std::vector<std::string>, std::vector<std::string>>(schemaX, schemaY);
    }
    auto getAttrSizeList(const std::vector<std::string> &schemas) const
    {
        std::deque<int> attrSizeList;
        int size = 0;
        for (const auto &s : schemas)
        {
            for (auto c = s.rbegin(); c != s.rend(); ++c)
            {
                if (*c == '#')
                {
                    if (size > 0 && *std::prev(c) == '0')
                    {
                        attrSizeList.emplace_back(size);
                        size = 0;
                    }
                    break;
                }
            }
            ++size;
        }
        attrSizeList.emplace_back(size);
        return attrSizeList;
    }
    auto getBitvector(
        const std::vector<std::vector<Share>> &table, const std::deque<int> &attrSizeList
    ) const
    {
        std::vector<std::vector<std::vector<Share>>> S;
        S.reserve(attrSizeList.size());
        int from = 0;
        for (const auto &size : attrSizeList)
        {
            std::vector<std::vector<Share>> tmpRow;
            tmpRow.reserve(table.size());
            int to = from + size;
            for (int i = from; i < to; ++i)
            {
                std::vector<Share> tmp;
                tmp.reserve(size);
                for (const auto &row : table)
                {
                    tmp.emplace_back(row[i]);
                }
                tmpRow.emplace_back(tmp);
            }
            S.emplace_back(tmpRow);
            from += size;
        }
        return S;
    }

public:
    nlohmann::json compute(
        const std::vector<std::vector<Share>> &table,
        const std::vector<std::string> &schemas,
        const std::vector<std::list<int>> &arg
    )
    {
        // 入力parse
        auto [x, y] = parse_table(table, arg);
        auto [schemaX, schemaY] = parse_schema(schemas, arg);

        auto attrXSizeList = getAttrSizeList(schemaX);
        auto attrYSizeList = getAttrSizeList(schemaY);

        // 各属性の使用状況：初期値は全て使用可能で1
        auto T = std::vector<Share>(table.size(), qmpc::Share::getConstantShare(FixedPoint("1")));

        // 設計変数の属性ごとbitvector
        auto S = getBitvector(x, attrXSizeList);

        // 目的値のbitvector
        auto target = getBitvector(y, attrYSizeList)[0];

        // 属性集合：{0,1,...,attrSize}
        std::set<int> R;
        for (int i = 0; i < static_cast<int>(attrXSizeList.size()); ++i)
        {
            R.emplace(i);
        }

        auto tree = qmpc::GBDT::SID3::createTree(T, S, target, R);
        auto treeJson = tree->createTreeJson();
        return treeJson;
    }
};
}  // namespace qmpc::Job