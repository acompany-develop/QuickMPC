#include "join_table.hpp"

#include <openssl/sha.h>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <unordered_map>
#include <unordered_set>

#include "client/computation_to_db/client.hpp"
#include "job/progress_manager.hpp"
#include "logging/logger.hpp"
#include "share/compare.hpp"
#include "share/share.hpp"

namespace qmpc::ComputationToDb
{
using Share = ::Share;

template <class SV = FixedPoint>
auto toShare(const std::vector<std::string> &vec)
{
    auto sv_vec = std::vector<SV>(vec.begin(), vec.end());
    auto share_vec = std::vector<Share>(sv_vec.begin(), sv_vec.end());
    return share_vec;
}
template <class SV = FixedPoint>
auto toShare(const std::vector<std::vector<std::string>> &table)
{
    std::vector<std::vector<Share>> &share_table;
    share_table.reserve(table.size());
    for (const auto &row : table)
    {
        share_table.emplace_back(toShare<SV>(row));
    }
    return share_table;
}

std::string joinDataId(const ValueTable &vt1, const ValueTable &vt2, int type)
{
    // 結合テーブルIDと結合方式から一意に定まるIDを生成する
    auto text = vt1.getDataId() + vt2.getDataId() + std::to_string(type);

    unsigned char hs[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, text.c_str(), text.size());
    SHA256_Final(hs, &sha256);
    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hs[i]);
    }
    return ss.str();
}

struct LessFloat
{
    static constexpr double eps = 1e-5;
    template <class T>
    bool operator()(const T &x, const T &y) const
    {
        return x < y - T(eps);
    }
    bool operator()(const std::string &x, const std::string &y) const { return x < y; }
    bool operator()(const SchemaType &x, const SchemaType &y) const { return x < y; }
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
std::pair<std::vector<int>, std::vector<int>> intersectionSortedValueIndex(
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

    // intersection_v1_its, intersection_v2_its:= v1とv2の積集合のindex
    std::vector<int> intersection_v1_its;
    std::vector<int> intersection_v2_its;
    intersection_v1_its.reserve(size);
    intersection_v2_its.reserve(size);
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
                    intersection_v1_its.emplace_back(now_its[i].first);
                    intersection_v2_its.emplace_back(now_its[i].second);
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

    std::sort(intersection_v1_its.begin(), intersection_v1_its.end());
    std::sort(intersection_v2_its.begin(), intersection_v2_its.end());
    return {intersection_v1_its, intersection_v2_its};
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

std::string writeHJoinTable(
    const ValueTable &table1,
    const ValueTable &table2,
    const std::vector<int> &col1,
    const std::vector<int> &col2,
    const std::vector<int> &row1,
    const std::vector<int> &row2
)
{
    // tableをpieceごとに保存する機構
    const std::string new_data_id = joinDataId(table1, table2, 1);
    auto writer = TableWriter(new_data_id);
    writer.addMatchingColumn(table1.getMatchinColumnNumber());

    // schemasを構築
    auto new_schemas = std::vector<SchemaType>();
    auto schemas1 = table1.getSchemas();
    auto schemas2 = table2.getSchemas();
    for (const auto &it : col1)
    {
        new_schemas.emplace_back(schemas1[it]);
    }
    for (const auto &it : col2)
    {
        new_schemas.emplace_back(schemas2[it]);
    }
    writer.emplace(new_schemas);

    // tableを構築
    auto row_dq1 = std::deque<int>(row1.begin(), row1.end());
    auto row_dq2 = std::deque<int>(row2.begin(), row2.end());
    auto itr1 = table1.begin();
    auto itr2 = table2.begin();
    auto table_idx1 = 0;
    auto table_idx2 = 0;
    while (!row_dq1.empty() && !row_dq2.empty())
    {
        // 使用される行になるまでincrement
        while (table_idx1 < row_dq1.front())
        {
            ++table_idx1;
            ++itr1;
        }
        while (table_idx2 < row_dq2.front())
        {
            ++table_idx2;
            ++itr2;
        }
        if (itr1 == table1.end() || itr2 == table2.end())
        {
            qmpc::Log::throw_with_trace(
                std::range_error("The specified row is out of range of the table data.")
            );
        }

        // 行を構築
        auto new_row = std::vector<std::string>();
        new_row.reserve(new_schemas.size());
        auto r1 = *itr1;
        auto r2 = *itr2;
        for (const auto &it : col1)
        {
            new_row.emplace_back(r1[it]);
        }
        for (const auto &it : col2)
        {
            new_row.emplace_back(r2[it]);
        }

        writer.emplace(new_row);
        ++itr1;
        ++itr2;
        ++table_idx1;
        ++table_idx2;
        row_dq1.pop_front();
        row_dq2.pop_front();
    }

    writer.write();
    return new_data_id;
}

ValueTable hjoin(const ValueTable &table1, const ValueTable &table2)
{
    // joinしたschemasのindexリストを構築
    auto schemas_it1 = std::vector<int>(table1.getSchemas().size());
    auto schemas_it2 = std::vector<int>();
    std::iota(schemas_it1.begin(), schemas_it1.end(), 0);
    auto size2 = table2.getSchemas().size();
    auto matching_column2 = table2.getMatchinColumnNumber();
    for (size_t i = 0; i < size2; ++i)
    {
        if (static_cast<int>(i) == matching_column2 - 1)
        {
            continue;
        }
        schemas_it2.emplace_back(i);
    }

    // joinしたidsのindexリストを構築
    auto ids_share1 = toShare(table1.getIdColumn());
    auto ids1 = open_and_recons(ids_share1);
    auto ids_share2 = toShare(table2.getIdColumn());
    auto ids2 = open_and_recons(ids_share2);
    auto [ids_it1, ids_it2] = intersectionValueIndex(ids1, ids2);

    // tableをjoinして保存
    auto new_data_id = writeHJoinTable(table1, table2, schemas_it1, schemas_it2, ids_it1, ids_it2);
    return ValueTable(new_data_id);
}

ValueTable hjoinShare(const ValueTable &table1, const ValueTable &table2)
{
    // joinしたschemasのindexリストを構築
    auto schemas_it1 = std::vector<int>(table1.getSchemas().size());
    auto schemas_it2 = std::vector<int>();
    std::iota(schemas_it1.begin(), schemas_it1.end(), 0);
    auto size2 = table2.getSchemas().size();
    auto matching_column2 = table2.getMatchinColumnNumber();
    for (size_t i = 0; i < size2; ++i)
    {
        if (static_cast<int>(i) == matching_column2 - 1)
        {
            continue;
        }
        schemas_it2.emplace_back(i);
    }

    // joinしたidsのindexリストを構築
    auto ids_share1 = toShare(table1.getIdColumn());
    auto ids_share2 = toShare(table2.getIdColumn());
    auto [ids_it1, ids_it2] = intersectionSortedValueIndex(ids_share1, ids_share2);

    // tableをjoinして保存
    auto new_data_id = writeHJoinTable(table1, table2, schemas_it1, schemas_it2, ids_it1, ids_it2);
    return ValueTable(new_data_id);
}

auto parseRead(const std::vector<ValueTable> &values, bool debug_mode)
{
    auto joinFunc = [&](auto &&f, const ValueTable &t, unsigned int it = 1)
    {
        if (it == values.size())
        {
            return t;
        }
        if (debug_mode)
        {
            return f(f, hjoin(t, values[it]), it + 1);
        }
        else
        {
            return f(f, hjoinShare(t, values[it]), it + 1);
        }
    };
    return joinFunc(joinFunc, values[0]);
}

// tableデータを結合して取り出す
ValueTable readTable(const managetocomputation::JoinOrder &table)
{
    // requestからデータ読み取り
    std::vector<ValueTable> tables;
    for (const auto &data_id : table.data_ids())
    {
        tables.emplace_back(data_id);
    }
    bool debug_mode = table.debug_mode();
    return parseRead(tables, debug_mode);
}

}  // namespace qmpc::ComputationToDb
