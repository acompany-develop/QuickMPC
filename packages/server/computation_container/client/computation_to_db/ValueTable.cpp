#include "ValueTable.hpp"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <unordered_map>
#include <unordered_set>

#include "job/ProgressManager.hpp"
#include "Share/Compare.hpp"

namespace qmpc::ComputationToDb
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
    // v1, v2がソートされている必要あり
    if (sorted_v1.size() == 0 || sorted_v2.size() == 0)
    {
        return {};
    }

    auto progress_manager = qmpc::Job::ProgressManager::getInstance();
    const auto job_id = sorted_v1[0].getId().getJobId();
    auto core_progress =
        progress_manager->createProgress<qmpc::Job::ProgressIters>(job_id, "hjoin: core", 4);

    int size = sorted_v1.size();
    int len = sorted_v2.size();
    // ブロックサイズ(S)と暫定のブロック数(N):N:=size/S
    // サイズO(N)のBulk比較をO(log(len) + S)回行う
    int block_size = std::min(100, size);
    int block_nums = (size + block_size - 1) / block_size;

    // 各ブロックの始点indexとその値
    std::vector<int> block_it;
    std::vector<T> block_s;
    block_it.reserve(block_nums);
    block_s.reserve(block_nums);
    for (int i = 0; i < block_nums; ++i)
    {
        auto it = i * block_size;
        block_it.emplace_back(it);
        block_s.emplace_back(sorted_v1[it]);
    }

    // parallel binary search
    // block_itの各要素 i について(v1[i]>=v2[j]) となる最小のjを求める
    // 求めた j はupperに格納される
    // 各iとjはv1,v2のブロックの区切りをなす
    std::vector<int> lower(block_nums, -1);
    std::vector<int> upper(block_nums, len - 1);
    int iterated_num = std::log2(len) + 1;

    core_progress->update(1);
    {
        auto binary_search_progress = progress_manager->createProgress<qmpc::Job::ProgressIters>(
            job_id, "hjoin: binary search", iterated_num
        );
        for (int progress_i = 0; progress_i < iterated_num; ++progress_i)
        {
            std::vector<int> mid_v(block_nums);
            for (int i = 0; i < block_nums; ++i)
            {
                mid_v[i] = (lower[i] + upper[i]) / 2;
            }

            std::vector<T> target;
            target.reserve(block_nums);
            for (const auto &mid : mid_v)
            {
                target.emplace_back(sorted_v2[mid]);
            }

            auto less_eq = qmpc::Share::allLessEq(block_s, target);
            for (int i = 0; i < block_nums; ++i)
            {
                (less_eq[i] ? upper[i] : lower[i]) = mid_v[i];
            }
            binary_search_progress->update(progress_i);
        }
    }

    core_progress->update(2);

    // parallel linear search
    // 各ブロックを並列に走査する
    // sorted_v1は各ブロック i について [begin_its[i].first,  end_its[i].first ) を走査
    // sorted_v2は各ブロック i について [begin_its[i].second, end_its[i].second) を走査
    std::vector<std::pair<int, int>> begin_its;
    std::vector<std::pair<int, int>> end_its;
    begin_its.reserve(block_nums);
    end_its.reserve(block_nums);

    for (int i = 0; i < block_nums; ++i)
    {
        auto v1_l = block_it[i];
        auto v1_r = (i + 1 < block_nums) ? block_it[i + 1] : size;
        auto v2_l = std::max(0, upper[i]);
        auto v2_r = (i + 1 < block_nums) ? std::max(0, upper[i + 1]) : len;
        begin_its.emplace_back(v1_l, v2_l);
        end_its.emplace_back(v1_r, v2_r);
    }
    auto now_its = begin_its;
    core_progress->update(3);

    // intersection_it_list:= v1とv2の積集合のindex
    std::vector<std::pair<int, int>> intersection_it_list;
    intersection_it_list.reserve(size);
    // 並列比較するブロックの数
    int parallel_nums = now_its.size();
    std::vector<bool> fin(block_nums, false);
    {
        // 進捗最大値は動的に変わりうるため便宜上100に固定
        auto linear_search_progress = progress_manager->createProgress<qmpc::Job::ProgressIters>(
            job_id, "hjoin: linear search", 100
        );
        while (true)
        {
            // l < r を実行したい全てのl, rをcomp_l,comp_rに入れる
            std::vector<T> comp_l;
            std::vector<T> comp_r;
            comp_l.reserve(parallel_nums);
            comp_r.reserve(parallel_nums);
            for (int i = 0; i < parallel_nums; ++i)
            {
                // 不要な比較を減らすため探索し終えたindexを見ない
                if (fin[i])
                {
                    continue;
                }
                auto [i1, i2] = now_its[i];
                auto [lim1, lim2] = end_its[i];
                if (i1 == lim1 || i2 == lim2)
                {
                    fin[i] = true;
                    continue;
                }
                // lessとgreaterも並列で行う
                comp_l.emplace_back(sorted_v1[i1]);
                comp_l.emplace_back(sorted_v2[i2]);
                comp_r.emplace_back(sorted_v2[i2]);
                comp_r.emplace_back(sorted_v1[i1]);
            }

            // comp_lが空 <=> 全てのindexでendまで探索し終えた
            if (comp_l.empty())
            {
                break;
            }

            // [0th less, 0th greater, 1st less, 1st greater ....]
            auto comp = qmpc::Share::allLess(comp_l, comp_r);
            auto comp_it = 0;
            int min_progress = 100;
            for (int i = 0; i < parallel_nums; ++i)
            {
                if (fin[i])
                {
                    continue;
                }
                // "==" <=> "not < && not >"
                if (!comp[comp_it] && !comp[comp_it + 1])
                {
                    intersection_it_list.emplace_back(now_its[i]);
                    ++now_its[i].first;
                    ++now_its[i].second;
                }
                else if (comp[comp_it])
                {
                    ++now_its[i].first;
                }
                else
                {
                    ++now_its[i].second;
                }

                // 1回の等値比較でlessとgreaterの2つ取り出すので2つ進める
                comp_it += 2;

                // begin->endに何%移動しているか
                // v1,v2の大さい方をこのindexでの進捗とする(どちらかが100%で終了のため)
                auto [b1, b2] = begin_its[i];
                auto [e1, e2] = end_its[i];
                auto [i1, i2] = now_its[i];
                auto progress1 = 100 * (i1 - b1) / (e1 - b1);
                auto progress2 = 100 * (i2 - b2) / (e2 - b2);
                min_progress = std::min(min_progress, std::max(progress1, progress2));
            }
            linear_search_progress->update(min_progress);
        }
    }

    return intersection_it_list;
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
    auto schemas_it = std::vector<int>(schemas.size());
    std::iota(schemas_it.begin(), schemas_it.end(), 0);
    auto schemas_join_it = std::vector<int>();
    schemas_join_it.reserve(join_table.schemas.size());
    for (size_t i = 0; i < join_table.schemas.size(); ++i)
    {
        if (static_cast<int>(i) == idx_tgt - 1)
        {
            continue;
        }
        schemas_join_it.emplace_back(i);
    }

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
    auto joinFunc = [&](auto &&f, const qmpc::ComputationToDb::ValueTable &t, unsigned int it = 0)
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
    auto schemas_it = std::vector<int>(schemas.size());
    std::iota(schemas_it.begin(), schemas_it.end(), 0);
    auto schemas_join_it = std::vector<int>();
    schemas_join_it.reserve(join_table.schemas.size());
    for (size_t i = 0; i < join_table.schemas.size(); ++i)
    {
        if (static_cast<int>(i) == idx_tgt - 1)
        {
            continue;
        }
        schemas_join_it.emplace_back(i);
    }

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
}  // namespace qmpc::ComputationToDb
