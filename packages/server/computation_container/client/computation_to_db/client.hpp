#pragma once

#include <experimental/filesystem>
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
namespace fs = std::experimental::filesystem;
using SchemaType = std::tuple<std::string, pb_common_types::ShareValueTypeEnum>;

std::vector<nlohmann::json>
convertSchemaVectorToJsonVector(const std::vector<qmpc::ComputationToDb::SchemaType> &);

class Client final
{
    static inline const auto shareDbPath = fs::path("/db/share/");
    static inline const auto resultDbPath = fs::path("/db/result/");

public:
    Client();
    static std::shared_ptr<Client> getInstance();

    // Tableの取り出し
    std::optional<std::vector<std::vector<std::string>>> readTable(const std::string &, int) const;
    std::vector<SchemaType> readSchema(const std::string &) const;

    // shareDBに対してdataを書き込む
    void writeShareDB(const std::string &data_id, const std::string &data, int piece_id = 0);
    // resultDBに対してdataを書き込む
    void writeResultDB(
        const std::string &job_uuid, const std::string &data, int data_type, int piece_id = 0
    );

    // Job を DB に新規登録する
    void registerJob(const std::string &job_uuid, const int &status) const;

    // Job の実行状態を更新する
    void updateJobStatus(const std::string &job_uuid, const int &status) const;

    // Job の完了を登録する
    void updateJobCompleted(const std::string &job_uuid) const;

    // Job 実行中に発生したエラーに関する情報を保存する
    void saveErrorInfo(const std::string &job_uuid, const pb_common_types::JobErrorInfo &info)
        const;
};

// 計算結果のWriter
class ComputationResultWriter
{
    int current_size;
    int piece_id;
    std::vector<std::string> piece_data;

    const std::string job_uuid;
    const int data_type;
    const int column_number;
    const int piece_size;

public:
    ComputationResultWriter(const std::string &, int, int, int piece_size = 1000000);

    // resultの保存
    void emplace(const std::string &);
    void emplace(const std::vector<std::string> &);
    void emplace(const std::vector<std::vector<std::string>> &);
    void emplace(const SchemaType &);
    void emplace(const std::vector<SchemaType> &);

    void write(bool fin = true);
};

// TableWriterとComputationResultWriterは本質的には同じことをしているが，
// 微妙に形式が異なるため別の実装としている
// 例)send_share時は行ごとにpieceとするが，計算結果は値ごとにpieceとしている
class TableWriter
{
    int current_size;
    int piece_id;
    std::vector<std::vector<std::string>> piece_data;
    nlohmann::json json_schemas;

    const std::string data_id;
    const int piece_size;

public:
    TableWriter(const std::string &, int piece_size = 1000000);

    void write();

    void emplace(const std::vector<std::string> &);
    void emplace(const std::vector<SchemaType> &);
};

}  // namespace qmpc::ComputationToDb
