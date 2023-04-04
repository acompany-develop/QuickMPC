#pragma once

#include <fstream>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "external/proto/common_types/common_types.pb.h"
#include "logging/logger.hpp"
#include "nlohmann/json.hpp"

namespace qmpc::ComputationToDb
{
using SchemaType = std::tuple<std::string, pb_common_types::ShareValueTypeEnum>;

std::vector<nlohmann::json>
convertSchemaVectorToJsonVector(const std::vector<qmpc::ComputationToDb::SchemaType> &);

class Client final
{
    static inline const std::string shareDbPath = "/db/share/";
    static inline const std::string resultDbPath = "/db/result/";

    class ComputationResultWriter
    {
        int current_size;
        int piece_id;
        std::vector<std::string> piece_data;

        const std::string job_uuid;
        const std::string data_name;
        const int column_number;
        const int piece_size;

    public:
        ComputationResultWriter(const std::string &, int, int, int);
        void write();

        void emplace(const std::string &);
        void emplace(const std::vector<std::string> &);
        void emplace(const SchemaType &);
    };

public:
    Client();
    static std::shared_ptr<Client> getInstance();

    // Tableの取り出し
    std::optional<std::vector<std::vector<std::string>>> readTable(const std::string &, int) const;
    std::vector<SchemaType> readSchema(const std::string &) const;

    // Tableの保存
    std::string
    writeTable(const std::string &, std::vector<std::vector<std::string>> &, const std::vector<SchemaType> &)
        const;

    // Job を DB に新規登録する
    void registerJob(const std::string &job_uuid, const int &status) const;

    // Job の実行状態を更新する
    void updateJobStatus(const std::string &job_uuid, const int &status) const;

    // Job 実行中に発生したエラーに関する情報を保存する
    void saveErrorInfo(const std::string &job_uuid, const pb_common_types::JobErrorInfo &info)
        const;

    // 保存時にデフォルトで呼ばれる恒等関数
    static inline auto identity = [](const auto &t) { return t; };

    // resultの保存
    // NOTE: result_listにbegin()とend()が実装されている必要がある
    template <class T, class F = decltype(identity)>
    void writeComputationResult(
        const std::string &job_uuid,
        const T &result_list,
        int data_type,  // 0:dim1, 1:dim2, 2:schema
        int column_number,
        const F &f = identity,  // 保存時にitrごとに加工したい場合に指定する
        int piece_size = 1000000
    ) const
    {
        auto writer = ComputationResultWriter(job_uuid, data_type, column_number, piece_size);
        for (const auto &x : result_list)
        {
            writer.emplace(f(x));
        }
        writer.write();
        std::ofstream(resultDbPath + job_uuid + "/completed");
    }

    // TableWriteとComputationResultWriteは本質的には同じことをしているが，
    // 微妙に形式が異なるため別の実装としている
    // 例)send_share時は行ごとにpieceとするが，計算結果は値ごとにpieceとしている
    class TableWrite
    {
        int current_size;
        int piece_id;
        std::vector<std::vector<std::string>> piece_data;
        nlohmann::json json_schemas;

        const std::string data_id;
        const int piece_size;

    public:
        TableWrite(const std::string &, int piece_size = 1000000);

        void write();

        void emplace(const std::vector<std::string> &);
        void emplace(const std::vector<SchemaType> &);
    };
};
}  // namespace qmpc::ComputationToDb
