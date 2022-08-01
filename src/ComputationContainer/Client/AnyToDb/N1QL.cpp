#include "N1QL.hpp"

#include <chrono>
#include <regex>

#include "LogHeader/Logger.hpp"

namespace AnyToDb
{
    /* N1QlValue statementの実装 */
    N1QLValue::N1QLValue(const std::string &value, const std::string &key) : value(value), key(key) {}

    auto N1QLValue::toValueSetString() const -> std::string
    {
        return "(\"" + key + "\"," + value + ")";
    }

    /* N1Ql statementの実装 */
    N1QL::N1QL(const std::string &bucket) : clause(bucket) {}

    auto N1QL::insert(const N1QLValue &data) const -> std::string
    {
        return clause.insert(data.toValueSetString()) +
               clause.returning();
    }

    auto N1QL::bulkinsert(const std::vector<N1QLValue> &datas) const -> std::string
    {
        // バルク用のstring
        std::string values;

        for (std::size_t i = 0; i < datas.size(); i++)
        {
            values += datas[i].toValueSetString();
            if (i != datas.size() - 1)
            {
                values += ",";
            }
        }

        return clause.insert(values) +
               clause.returning();
    }

    auto N1QL::update(
        const std::pair<std::string, std::string> &cond,
        const std::pair<std::string, std::string> &path_expr
    ) const -> std::string
    {
        return clause.update(path_expr.first, path_expr.second) +
               clause.where(cond.first, cond.second) +
               clause.returning();
    }

    auto N1QL::select() const -> std::string
    {
        return clause.select();
    }

    auto N1QL::select_id(const std::string &id_name, const std::string &id) const -> std::string
    {
        return clause.select() +
               clause.where(id_name, id);
    }

    auto N1QL::delete_id(const std::string &id_name, const std::string &id) const -> std::string
    {
        return clause.delete_f() +
               clause.where(id_name, id) +
               clause.returning();
    }

    auto N1QL::delete_order(unsigned int amount) const -> std::string
    {
        return clause.delete_f() +
               clause.limit(amount) +
               clause.returning();
    }

    /* N1QL clauseの実装 */
    N1QL::N1QLClause::N1QLClause(const std::string &bucket) : bucket(bucket) {}

    auto N1QL::N1QLClause::insert(const std::string &value) const -> std::string
    {
        return "INSERT INTO `" + bucket + "` (KEY, VALUE) VALUES " + value;
    }

    auto N1QL::N1QLClause::update(const std::string &path, const std::string &expr) const -> std::string
    {
        return "UPDATE `" + bucket + "` x SET x." +
               path +
               " = " +
               expr +
               " ";
    }

    auto N1QL::N1QLClause::select() const -> std::string
    {
        return "SELECT x.*, meta().id FROM `" + bucket + "` x ";
    }

    auto N1QL::N1QLClause::delete_f() const -> std::string
    {
        return "DELETE FROM `" + bucket + "` x ";
    }

    auto N1QL::N1QLClause::where(const std::string &key, const std::string &data) const -> std::string
    {
        return "WHERE x." +
               escape_injection(key, N1QL_WHERE_PARAM) +
               " = \"" +
               escape_injection(data, N1QL_WHERE_PARAM) +
               "\" ";
    }

    auto N1QL::N1QLClause::returning() const -> std::string
    {
        return "RETURNING x.*, meta().id ";
    }

    auto N1QL::N1QLClause::limit(unsigned int lim) const -> std::string
    {
        return "LIMIT " + std::to_string(lim) + " ";
    }

    /*
    N1QLインジェクション対策用パラメータ埋め込み前処理関数
    param_typeでパラメータの種類を指定するとサニタイズを行う
    N1QLのメソッドを追加する開発者はこの関数でインジェクション対策を行う必要がある
    */
    auto N1QL::N1QLClause::escape_injection(const std::string &target, const n1ql_param_type param_type) const -> std::string
    {
        switch (param_type)
        {
        case N1QL_SELECT_PARAM:
            return std::regex_replace(
                target,
                std::regex("`"), "`");

        case N1QL_WHERE_PARAM:
            return std::regex_replace(
                std::regex_replace(target, std::regex("'"), "''"),
                std::regex("\""), "\"\"");

        default:
            // 上記ケース以外は無効なのでエラーを起こす
            // 現在の実装では，ここに到達する可能性はない
            const std::string msg = "An invalid clause type other than SELECT or WHERE clause was passed during injection escaping";
            spdlog::error(msg);
            throw std::invalid_argument(msg);
        }
    }

    /* my cas */
    // タイムスタンプを返す
    uint64_t getTimeStamp()
    {
        // 現在日時を取得
        const std::chrono::system_clock::time_point p = std::chrono::system_clock::now();

        // エポックからの経過時間(ナノ秒)を取得
        const std::chrono::nanoseconds nsec = std::chrono::duration_cast<std::chrono::nanoseconds>(p.time_since_epoch());

        const uint64_t timestamp = nsec.count();

        return timestamp;
    }

    // 論理カウンタを返す
    uint16_t getLogicalCounter()
    {
        static uint16_t counter = 0;
        return counter++;
    }

    // とりあえずcasを生成する
    uint64_t createCas()
    {
        const uint64_t timestamp = getTimeStamp();
        const uint16_t logicalcounter = getLogicalCounter();
        // 64bit timestampの下位16bitを切り捨てるために使用するビットマスク
        constexpr uint64_t MASK = 0xffffffffffff0000ULL;
        // 64bit timestampの下位16bitを捨てたものとlogicalcounterの下位16bitをOR
        // logicalcounterはuint16_tなのでビットマスクは不要
        const uint64_t cas = (MASK & timestamp) | logicalcounter;

        return cas;
    }

    // 推移律が成り立つcasを返す
    uint64_t getCas()
    {
        static uint64_t precas = 0;
        uint64_t cas = createCas();

        while (precas != 0 && cas < precas)
        {
            cas = createCas();
        }

        precas = cas;

        return cas;
    }
} // namespace AnyToDb
