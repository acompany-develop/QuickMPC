#include "ValueTable.hpp"

#include <algorithm>
#include <chrono>
#include <unordered_map>
#include <unordered_set>

#include "Share/Compare.hpp"

namespace qmpc::ComputationToDbGate
{
ValueTable::ValueTable(
    const std::vector<std::vector<std::string>> &table, const std::vector<std::string> &schemas
)
    : table(table), schemas(schemas)
{
}
bool ValueTable::operator==(const ValueTable &that) const
{
    return (table == that.table) & (schemas == that.schemas);
}

std::vector<::Share> ValueTable::getColumn(int idx) const
{
    std::vector<Share> column;
    column.reserve(table.size());
    for (const auto &row : table)
    {
        column.emplace_back(FixedPoint(row[idx]));
    }
    return column;
}

std::vector<std::vector<std::string>> ValueTable::getTable() const { return table; }

std::vector<std::string> ValueTable::getSchemas() const { return schemas; }

struct LessFloat
{
    static constexpr double eps = 1e-5;
    template <class T>
    bool operator()(const T &x, const T &y) const
    {
        return x < y - T(eps);
    }
    bool operator()(const std::string &x, const std::string &y) const { return x < y; }
};

template <class T>
std::pair<std::vector<int>, std::vector<int>> intersectionValueIndex(
    const std::vector<T> &v1, const std::vector<T> &v2
)
{
    std::map<T, int, LessFloat> v2_mp;
    for (unsigned int it = 0; it < v2.size(); ++it)
    {
        v2_mp.emplace(v2[it], it);
    }
    std::vector<int> v1_it;
    std::vector<int> v2_it;
    v1_it.reserve(v1.size());
    v2_it.reserve(v2.size());
    for (unsigned int it = 0; it < v1.size(); ++it)
    {
        if (v2_mp.find(v1[it]) == v2_mp.end())
        {
            continue;
        }
        v1_it.emplace_back(it);
        v2_it.emplace_back(v2_mp[v1[it]]);
    }
    return {v1_it, v2_it};
}

template <class T>
std::vector<std::pair<int, int>> intersectionSortedValueIndex(
    const std::vector<T> &sorted_v1, const std::vector<T> &sorted_v2
)
{
    // v2がソートされている必要あり
    spdlog::info("[progress] hjoin: core (0/1)");

    int size = sorted_v1.size();
    int len = sorted_v2.size();
    std::vector<int> lower(size, -1);
    std::vector<int> upper(size, len - 1);
    auto d_max = len;
    // parallel binary search
    int iterated = 0;
    while (d_max > 1)
    {
        std::vector<int> mid_v(size);
        for (int i = 0; i < size; ++i)
        {
            mid_v[i] = (lower[i] + upper[i]) / 2;
        }

        std::vector<T> target;
        target.reserve(size);
        for (const auto &mid : mid_v)
        {
            target.emplace_back(sorted_v2[mid]);
        }

        auto less_eq = qmpc::Share::allLessEq(sorted_v1, target);
        d_max = 0;
        for (int i = 0; i < size; ++i)
        {
            (less_eq[i] ? upper[i] : lower[i]) = mid_v[i];
            d_max = std::max(d_max, upper[i] - lower[i]);
        }
        double progress = 100.0 - d_max * 100.0 / len;
        spdlog::info("[progress] hjoin: core (0/1): {:>5.2f} %", progress);
    }

    std::vector<T> target;
    target.reserve(size);
    for (const auto &i : upper)
    {
        target.emplace_back(std::max(0, i));
    }
    spdlog::info("[progress] hjoin: core (0/1): {:>5.2f} %", 100.0);

    // leq && geq => eq
    auto greater_eq = qmpc::Share::allGreaterEq(sorted_v1, target);
    std::vector<std::pair<int, int>> it_list;
    it_list.reserve(sorted_v1.size());
    for (int i = 0; i < size; ++i)
    {
        if (greater_eq[i])
        {
            it_list.emplace_back(i, upper[i]);
        }
    }

    spdlog::info("[progress] hjoin: core (1/1)");
    return it_list;
}

template <class T>
std::pair<std::vector<int>, std::vector<int>> unionValueIndex(
    const std::vector<T> &v1, const std::vector<T> &v2
)
{
    std::vector<int> v1_it;
    v1_it.reserve(v1.size());
    std::set<T, LessFloat> v1_st;
    for (unsigned int it = 0; it < v1.size(); ++it)
    {
        v1_it.emplace_back(it);
        v1_st.emplace(v1[it]);
    }
    std::vector<int> v2_it;
    v2_it.reserve(v2.size());
    for (unsigned int it = 0; it < v2.size(); ++it)
    {
        if (v1_st.find(v2[it]) != v1_st.end())
        {
            continue;
        }
        v2_it.emplace_back(it);
    }
    return {v1_it, v2_it};
}

ValueTable ValueTable::vjoin(const ValueTable &join_table, int idx, int idx_tgt) const
{
    // joinしたschemasを構築
    auto [schemas_it, schemas_join_it] = intersectionValueIndex(schemas, join_table.schemas);
    auto new_schemas_size = schemas_it.size();
    auto new_schemas = std::vector<std::string>();
    new_schemas.reserve(new_schemas_size);
    for (const auto &it : schemas_it)
    {
        new_schemas.emplace_back(schemas[it]);
    }

    // joinしたidsを構築
    auto ids_share = getColumn(idx - 1);
    open(ids_share);
    auto ids = recons(ids_share);
    auto ids_tgt_share = join_table.getColumn(idx_tgt - 1);
    open(ids_tgt_share);
    auto ids_tgt = recons(ids_tgt_share);
    auto [ids_it, ids_join_it] = unionValueIndex(ids, ids_tgt);
    auto new_ids_size = ids_it.size() + ids_join_it.size();

    // joinしたtableを構築
    auto new_table = std::vector<std::vector<std::string>>();
    new_table.reserve(new_ids_size);
    for (const auto &row : table)
    {
        auto new_row = std::vector<std::string>();
        new_row.reserve(new_schemas_size);
        for (const auto &it_w : schemas_it)
        {
            new_row.emplace_back(row[it_w]);
        }
        new_table.emplace_back(new_row);
    }
    for (const auto &it_h : ids_join_it)
    {
        auto new_row = std::vector<std::string>();
        new_row.reserve(new_schemas_size);
        for (const auto &it_w : schemas_join_it)
        {
            new_row.emplace_back(join_table.table[it_h][it_w]);
        }
        new_table.emplace_back(new_row);
    }

    return ValueTable(new_table, new_schemas);
}

ValueTable ValueTable::hjoin(const ValueTable &join_table, int idx, int idx_tgt) const
{
    // joinしたschemasを構築
    auto [schemas_it, schemas_join_it] = unionValueIndex(schemas, join_table.schemas);
    auto new_schemas_size = schemas_it.size() + schemas_join_it.size();
    auto new_schemas = schemas;
    new_schemas.reserve(new_schemas_size);
    for (const auto &it : schemas_join_it)
    {
        new_schemas.emplace_back(join_table.schemas[it]);
    }

    // joinしたidsを構築
    auto ids_share = getColumn(idx - 1);
    open(ids_share);
    auto ids = recons(ids_share);
    auto ids_tgt_share = join_table.getColumn(idx_tgt - 1);
    open(ids_tgt_share);
    auto ids_tgt = recons(ids_tgt_share);
    auto [ids_it, ids_join_it] = intersectionValueIndex(ids, ids_tgt);
    auto new_ids_size = ids_it.size();

    // joinしたtableを構築
    auto new_table = std::vector<std::vector<std::string>>();
    new_table.reserve(new_ids_size);
    auto it_h = ids_it.begin();
    auto it_join_h = ids_join_it.begin();
    while (it_h != ids_it.end())
    {
        auto new_row = std::vector<std::string>();
        new_row.reserve(new_schemas_size);
        for (const auto &it_w : schemas_it)
        {
            new_row.emplace_back(table[*it_h][it_w]);
        }
        for (const auto &it_w : schemas_join_it)
        {
            new_row.emplace_back(join_table.table[*it_join_h][it_w]);
        }
        new_table.emplace_back(new_row);
        ++it_h;
        ++it_join_h;
    }

    return ValueTable(new_table, new_schemas);
}

ValueTable parseRead(
    const std::vector<ValueTable> &values,
    const std::vector<int> &join,
    const std::vector<int> &index
)
{
    auto joinFunc =
        [&](auto &&f, const qmpc::ComputationToDbGate::ValueTable &t, unsigned int it = 0)
    {
        if (it == join.size())
        {
            return t;
        }
        if (join[it] == 0)
        {
            return f(f, t.hjoin(values[it + 1], index[0], index[it + 1]), it + 1);
        }
        if (join[it] == 1)
        {
            return f(f, t.vjoin(values[it + 1], index[0], index[it + 1]), it + 1);
        }
        return f(f, t.hjoinShare(values[it + 1], index[0], index[it + 1]), it + 1);
    };
    return joinFunc(joinFunc, values[0]);
}

ValueTable ValueTable::hjoinShare(const ValueTable &join_table, int idx, int idx_tgt) const
{
    // joinしたschemasを構築
    auto [schemas_it, schemas_join_it] = unionValueIndex(schemas, join_table.schemas);
    auto new_schemas_size = schemas_it.size() + schemas_join_it.size();
    auto new_schemas = schemas;
    new_schemas.reserve(new_schemas_size);
    for (const auto &it : schemas_join_it)
    {
        new_schemas.emplace_back(join_table.schemas[it]);
    }

    // joinしたidsを構築
    auto ids_share = getColumn(idx - 1);
    auto ids_tgt_share = join_table.getColumn(idx_tgt - 1);
    auto ids_it_list = intersectionSortedValueIndex(ids_share, ids_tgt_share);
    auto new_ids_size = ids_it_list.size();

    // joinしたtableを構築
    auto new_table = std::vector<std::vector<std::string>>();
    new_table.reserve(new_ids_size);
    for (const auto &[it_h, it_join_h] : ids_it_list)
    {
        auto new_row = std::vector<std::string>();
        new_row.reserve(new_schemas_size);
        for (const auto &it_w : schemas_it)
        {
            new_row.emplace_back(table[it_h][it_w]);
        }
        for (const auto &it_w : schemas_join_it)
        {
            new_row.emplace_back(join_table.table[it_join_h][it_w]);
        }
        new_table.emplace_back(new_row);
    }

    return ValueTable(new_table, new_schemas);
}
}  // namespace qmpc::ComputationToDbGate
