#pragma once

#include <string>
#include <vector>

namespace AnyToDb
{
    // 推移律が成り立つcasを返す
    uint64_t getCas();

    /*
    N1QLのクエリに埋め込むvalueの型
    keyはid
    valueはDbに入れる値で
    (e.g. jsonstrings ("{"a":123,"b":23,"c":1}""),
            simple value ("234"))
    といった形にする
    valueはどの形式で来るか不明なのでとりあえずstringで
    */
    class N1QLValue
    {
        const std::string value;
        const std::string key;

    public:
        N1QLValue() = delete;
        N1QLValue(const std::string &value, const std::string &key = std::to_string(getCas()));

        auto toValueSetString() const -> std::string;
    };

    // statement
    class N1QL
    {

        // clause
        class N1QLClause
        {
            const std::string bucket;
            typedef enum
            {
                N1QL_SELECT_PARAM = 0,
                N1QL_WHERE_PARAM = 1,
            } n1ql_param_type;

        public:
            N1QLClause() = delete;
            N1QLClause(const std::string &bucket);

            auto insert(const std::string &) const -> std::string;
            auto update(const std::string &, const std::string &) const -> std::string;
            auto select() const -> std::string;
            auto select_count() const -> std::string;
            auto delete_f() const -> std::string;
            auto where(const std::string &, const std::string &) const -> std::string;
            auto returning() const -> std::string;
            auto limit(unsigned int lim) const -> std::string;
            auto escape_injection(const std::string &, const n1ql_param_type param_type) const -> std::string;
        };

        const N1QLClause clause;

    public:
        N1QL() = delete;
        N1QL(const std::string &bucket);

        // `bucket`にdataを新規保存するクエリ
        // N1QLがstring一つでもコンストラクタが走るので，string単体なら引数に入れられる
        auto insert(const N1QLValue &) const -> std::string;
        // `bucket`にvaluesを一括で保存するクエリ
        auto bulkinsert(const std::vector<N1QLValue> &) const -> std::string;
        /// `bucket`のデータを更新するクエリ
        /// @param cond      ".first"が".second"であるデータに対して処理を行う
        /// @param path_expr ".first"を".second"の値に更新する
        auto update(
            const std::pair<std::string, std::string> &cond,
            const std::pair<std::string, std::string> &path_expr
        ) const -> std::string;
        // "バケット内のすべてのデータを参照するクエリ
        auto select() const -> std::string;
        // "id_name"が"id"であるデータを参照するクエリ
        auto select_id(const std::string &, const std::string &) const -> std::string;
        // "id_name"が"id"であるデータを削除するクエリ
        auto delete_id(const std::string &, const std::string &) const -> std::string;
        // meta id昇順でデータをamount個削除するクエリ
        auto delete_order(unsigned int) const -> std::string;
    };

    // タイムスタンプを返す
    uint64_t getTimeStamp();

    // 論理カウンタを返す
    uint16_t getLogicalCounter();

    // とりあえずcasを生成する
    uint64_t createCas();

} // namespace AnyToDb
