#include "value_table.hpp"

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

TableJoiner::TableJoiner(const std::string &data_id) : data_id(data_id) {}

// tableから行，列の指定したindexだけ取り出す，nulloptの場合は全ての行，列を取り出す
std::vector<std::vector<std::string>> TableJoiner::getSubTable(
    const std::optional<std::vector<int>> &row_index,
    const std::optional<std::vector<int>> &column_index
) const
{
    auto db = Client::getInstance();
    int piece_id = 0;

    // TODO: rowについての処理を書く
    int row = 0;
    std::vector<std::vector<std::string>> read_table;
    while (true)
    {
        auto piece = db->readTable(data_id, piece_id);
        if (!piece)
        {
            break;
        }
        for (const auto &row : piece.value())
        {
            if (!column_index)
            {
                read_table.emplace_back(row);
            }
            else
            {
                std::vector<std::string> row_c;
                for (const auto &idx : column_index.value())
                {
                    row_c.emplace_back(row[idx]);
                }
                read_table.emplace_back(row_c);
            }
        }
        ++piece_id;
    }
    return read_table;
}

std::vector<std::string> TableJoiner::getColumn(int column_number) const
{
    auto table = getSubTable(std::nullopt, std::vector<int>{column_number});
    std::vector<std::string> ret;
    for (const auto &row : table)
    {
        ret.emplace_back(row[0]);
    }
    return ret;
}
std::string TableJoiner::joinDataId(const TableJoiner &vt, int type) const
{
    // 結合テーブルIDと結合方式から一意に定まるIDを生成する
    auto text = data_id + vt.data_id + std::to_string(type);

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

std::string TableJoiner::getDataId() const { return data_id; }
std::vector<std::vector<std::string>> TableJoiner::getTable() const
{
    auto db = Client::getInstance();
    int piece_id = 0;
    std::vector<std::vector<std::string>> read_table;
    while (true)
    {
        auto piece = db->readTable(data_id, piece_id);
        if (!piece)
        {
            break;
        }
        for (const auto &row : piece.value())
        {
            read_table.emplace_back(row);
        }
        ++piece_id;
    }
    return read_table;
}

std::vector<std::string> TableJoiner::getSchemas() const
{
    auto db = Client::getInstance();
    return db->readSchema(data_id);
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

TableJoiner TableJoiner::vjoin(const TableJoiner &join_table, int idx, int idx_tgt) const
{
    // TODO: 必要な列だけ取り出して結合するようにする
    auto table = this->getTable();
    auto schemas = this->getSchemas();

    // joinしたschemasを構築
    auto [schemas_it, schemas_join_it] = intersectionValueIndex(schemas, join_table.getSchemas());
    auto new_schemas_size = schemas_it.size();
    auto new_schemas = std::vector<std::string>();
    new_schemas.reserve(new_schemas_size);
    for (const auto &it : schemas_it)
    {
        new_schemas.emplace_back(schemas[it]);
    }

    // joinしたidsを構築
    auto ids_share = toShare(getColumn(idx - 1));
    open(ids_share);
    auto ids = recons(ids_share);
    auto ids_tgt_share = toShare(join_table.getColumn(idx_tgt - 1));
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
            new_row.emplace_back(join_table.getTable()[it_h][it_w]);
        }
        new_table.emplace_back(new_row);
    }
    const std::string new_data_id = joinDataId(join_table, 0);
    auto db = Client::getInstance();
    db->writeTable(new_data_id, new_table, new_schemas);
    return TableJoiner(new_data_id);
}

TableJoiner TableJoiner::hjoin(const TableJoiner &join_table, int idx, int idx_tgt) const
{
    // TODO: 必要な列だけ取り出して結合するようにする
    auto table = this->getTable();
    auto schemas = this->getSchemas();

    // joinしたschemasを構築
    auto schemas_it = std::vector<int>(schemas.size());
    std::iota(schemas_it.begin(), schemas_it.end(), 0);
    auto schemas_join_it = std::vector<int>();
    schemas_join_it.reserve(join_table.getSchemas().size());
    for (size_t i = 0; i < join_table.getSchemas().size(); ++i)
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
        new_schemas.emplace_back(join_table.getSchemas()[it]);
    }

    // joinしたidsを構築
    auto ids_share = toShare(getColumn(idx - 1));
    open(ids_share);
    auto ids = recons(ids_share);
    auto ids_tgt_share = toShare(join_table.getColumn(idx_tgt - 1));
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
            new_row.emplace_back(join_table.getTable()[*it_join_h][it_w]);
        }
        new_table.emplace_back(new_row);
        ++it_h;
        ++it_join_h;
    }
    const std::string new_data_id = joinDataId(join_table, 1);
    auto db = Client::getInstance();
    db->writeTable(new_data_id, new_table, new_schemas);
    return TableJoiner(new_data_id);
}

TableJoiner TableJoiner::hjoinShare(const TableJoiner &join_table, int idx, int idx_tgt) const
{
    // TODO: indexだけで処理できるように修正する
    auto table = this->getTable();
    auto schemas = this->getSchemas();

    // joinしたschemasを構築
    auto schemas_it = std::vector<int>(schemas.size());
    std::iota(schemas_it.begin(), schemas_it.end(), 0);
    auto schemas_join_it = std::vector<int>();
    schemas_join_it.reserve(join_table.getSchemas().size());
    for (size_t i = 0; i < join_table.getSchemas().size(); ++i)
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
        new_schemas.emplace_back(join_table.getSchemas()[it]);
    }

    // joinしたidsを構築
    auto ids_share = toShare(getColumn(idx - 1));
    auto ids_tgt_share = toShare(join_table.getColumn(idx_tgt - 1));
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
            new_row.emplace_back(join_table.getTable()[it_join_h][it_w]);
        }
        new_table.emplace_back(new_row);
    }
    const std::string new_data_id = joinDataId(join_table, 1);
    auto db = Client::getInstance();
    db->writeTable(new_data_id, new_table, new_schemas);
    return TableJoiner(new_data_id);
}

std::string writeVJoinTable(
    const ValueTable &table1,
    const ValueTable &table2,
    const std::vector<int> &col1,
    const std::vector<int> &col2,
    const std::vector<int> &row1,
    const std::vector<int> &row2
)
{
    // schemasを構築
    auto new_schemas = std::vector<std::string>();
    auto schemas1 = table1.getSchemas();
    for (const auto &it : col1)
    {
        new_schemas.emplace_back(schemas1[it]);
    }

    // tableを構築
    auto new_table = std::vector<std::vector<std::string>>();
    new_table.reserve(row1.size());
    auto row_dq1 = std::deque<int>(row1.begin(), row1.end());
    auto row_dq2 = std::deque<int>(row2.begin(), row2.end());
    auto itr1 = table1.begin();
    auto itr2 = table2.begin();
    auto table_idx1 = 0;
    auto table_idx2 = 0;
    while (!row_dq1.empty())
    {
        // 使用される行になるまでincrement
        while (table_idx1 < row_dq1.front())
        {
            ++table_idx1;
            ++itr1;
        }
        if (itr1 == table1.end())
        {
            // TODO: 適切なエラーを返す
            throw std::runtime_error("");
        }

        // 行を構築
        auto new_row = std::vector<std::string>();
        new_row.reserve(new_schemas.size());
        auto r1 = *itr1;
        for (const auto &it : col1)
        {
            new_row.emplace_back(r1[it]);
        }
        new_table.emplace_back(new_row);
        ++itr1;
        ++table_idx1;
        row_dq1.pop_front();
    }
    while (!row_dq2.empty())
    {
        // 使用される行になるまでincrement
        while (table_idx2 < row_dq2.front())
        {
            ++table_idx2;
            ++itr2;
        }
        if (itr2 == table2.end())
        {
            // TODO: 適切なエラーを返す
            throw std::runtime_error("");
        }

        // 行を構築
        auto new_row = std::vector<std::string>();
        new_row.reserve(new_schemas.size());
        auto r2 = *itr2;
        for (const auto &it : col2)
        {
            new_row.emplace_back(r2[it]);
        }

        new_table.emplace_back(new_row);
        ++itr2;
        ++table_idx2;
        row_dq2.pop_front();
    }

    // 保存
    const std::string new_data_id = "tmp";
    auto db = Client::getInstance();
    db->writeTable(new_data_id, new_table, new_schemas);
    return new_data_id;
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
    // schemasを構築
    auto new_schemas = std::vector<std::string>();
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

    // tableを構築
    auto new_table = std::vector<std::vector<std::string>>();
    new_table.reserve(row1.size());
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
            // TODO: 適切なエラーを返す
            throw std::runtime_error("");
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

        new_table.emplace_back(new_row);
        ++itr1;
        ++itr2;
        ++table_idx1;
        ++table_idx2;
        row_dq1.pop_front();
        row_dq2.pop_front();
    }

    // 保存
    const std::string new_data_id = "tmp";
    auto db = Client::getInstance();
    db->writeTable(new_data_id, new_table, new_schemas);
    return new_data_id;
}

ValueTable vjoin(const ValueTable &table1, const ValueTable &table2, int idx1, int idx2)
{
    // joinしたschemasのindexリストを構築
    auto [schemas_it1, schemas_it2] =
        intersectionValueIndex(table1.getSchemas(), table2.getSchemas());

    // joinしたidsのindexリストを構築
    auto ids_share1 = toShare(table1.getColumn(idx1 - 1));
    open(ids_share1);
    auto ids1 = recons(ids_share1);
    auto ids_share2 = toShare(table2.getColumn(idx2 - 1));
    open(ids_share2);
    auto ids2 = recons(ids_share2);
    auto [ids_it1, ids_it2] = unionValueIndex(ids1, ids2);

    // tableをjoinして保存
    auto new_data_id = writeVJoinTable(table1, table2, schemas_it1, schemas_it2, ids_it1, ids_it2);
    return ValueTable(new_data_id);
}

ValueTable hjoin(const ValueTable &table1, const ValueTable &table2, int idx1, int idx2)
{
    // joinしたschemasのindexリストを構築
    auto schemas_it1 = std::vector<int>(table1.getSchemas().size());
    auto schemas_it2 = std::vector<int>();
    std::iota(schemas_it1.begin(), schemas_it1.end(), 0);
    auto size2 = table2.getSchemas().size();
    for (size_t i = 0; i < size2; ++i)
    {
        if (static_cast<int>(i) == idx2 - 1)
        {
            continue;
        }
        schemas_it2.emplace_back(i);
    }

    // joinしたidsのindexリストを構築
    auto ids_share1 = toShare(table1.getColumn(idx1 - 1));
    open(ids_share1);
    auto ids1 = recons(ids_share1);
    auto ids_share2 = toShare(table2.getColumn(idx2 - 1));
    open(ids_share2);
    auto ids2 = recons(ids_share2);
    auto [ids_it1, ids_it2] = intersectionValueIndex(ids1, ids2);

    // tableをjoinして保存
    auto new_data_id = writeHJoinTable(table1, table2, schemas_it1, schemas_it2, ids_it1, ids_it2);
    return ValueTable(new_data_id);
}

ValueTable hjoinShare(const ValueTable &table1, const ValueTable &table2, int idx1, int idx2)
{
    // joinしたschemasのindexリストを構築
    auto schemas_it1 = std::vector<int>(table1.getSchemas().size());
    auto schemas_it2 = std::vector<int>();
    std::iota(schemas_it1.begin(), schemas_it1.end(), 0);
    auto size2 = table2.getSchemas().size();
    for (size_t i = 0; i < size2; ++i)
    {
        if (static_cast<int>(i) == idx2 - 1)
        {
            continue;
        }
        schemas_it2.emplace_back(i);
    }

    // joinしたidsのindexリストを構築
    auto ids_share1 = toShare(table1.getColumn(idx1 - 1));
    auto ids_share2 = toShare(table2.getColumn(idx2 - 1));
    // NOTE: 返り値をpairにしたい
    auto ids_it_list = intersectionSortedValueIndex(ids_share1, ids_share2);
    std::vector<int> ids_it1;
    std::vector<int> ids_it2;
    ids_it1.reserve(ids_it_list.size());
    ids_it2.reserve(ids_it_list.size());
    for (const auto &[it1, it2] : ids_it_list)
    {
        ids_it1.emplace_back(it1);
        ids_it2.emplace_back(it2);
    }

    // tableをjoinして保存
    auto new_data_id = writeHJoinTable(table1, table2, schemas_it1, schemas_it2, ids_it1, ids_it2);
    return ValueTable(new_data_id);
}

auto parseRead(
    const std::vector<ValueTable> &values,
    const std::vector<int> &join,
    const std::vector<int> &index
)
{
    auto joinFunc = [&](auto &&f, const ValueTable &t, unsigned int it = 0)
    {
        if (it == join.size())
        {
            return t;
        }
        if (join[it] == 0)
        {
            return f(f, hjoin(t, values[it + 1], index[0], index[it + 1]), it + 1);
        }
        if (join[it] == 1)
        {
            return f(f, vjoin(t, values[it + 1], index[0], index[it + 1]), it + 1);
        }
        return f(f, hjoinShare(t, values[it + 1], index[0], index[it + 1]), it + 1);
    };
    return joinFunc(joinFunc, values[0]);
}

// tableデータを結合して取り出す
ValueTable readTable(const managetocomputation::JoinOrder &table)
{
    // requestからデータ読み取り
    auto size = table.join().size();
    std::vector<int> join;
    join.reserve(size);
    for (const auto &j : table.join())
    {
        join.emplace_back(j);
    }
    std::vector<int> index;
    index.reserve(size);
    for (const auto &j : table.index())
    {
        index.emplace_back(j);
    }
    std::vector<ValueTable> tables;
    tables.reserve(size + 1);
    for (const auto &data_id : table.dataids())
    {
        tables.emplace_back(data_id);
    }
    return parseRead(tables, join, index);
}

}  // namespace qmpc::ComputationToDb
