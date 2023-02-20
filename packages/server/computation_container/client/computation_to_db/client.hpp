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
    };

public:
    Client();
    static std::shared_ptr<Client> getInstance();

    // Tableの取り出し
    std::optional<std::vector<std::vector<std::string>>> readTable(const std::string &, int) const;
    std::vector<std::string> readSchema(const std::string &) const;

    // Tableの保存
    std::string
    writeTable(const std::string &, std::vector<std::vector<std::string>> &, const std::vector<std::string> &)
        const;

    // Job を DB に新規登録する
    void registerJob(const std::string &job_uuid, const int &status) const;

    // Job の実行状態を更新する
    void updateJobStatus(const std::string &job_uuid, const int &status) const;

    // Job 実行中に発生したエラーに関する情報を保存する
    void saveErrorInfo(const std::string &job_uuid, const pb_common_types::JobErrorInfo &info)
        const;

    // resultの保存
    // NOTE: result_listにbegin()とend()が実装されている必要がある
    template <class T>
    void writeComputationResult2(
        const std::string &job_uuid,
        const T &result_list,
        int data_type,  // 0:dim1, 1:dim2, 2:schema
        int column_number,
        int piece_size = 1000000
    ) const
    {
        auto writer = ComputationResultWriter(job_uuid, data_type, column_number, piece_size);
        for (const auto &x : result_list)
        {
            writer.emplace(x);
        }
        writer.write();
        std::ofstream(resultDbPath + job_uuid + "/completed");
    }

    // resultの保存
    template <class T>
    void writeComputationResult(
        const std::string &job_uuid,
        const T &results,
        bool is_schema = false,
        int piece_size = 1000000
    ) const
    {
        // resultsの形式は1次元 or 2次元 or json

        // NOTE: 現バージョンでresultsがjsonとなるのは結合リクエストのみ
        // 結合リクエストの場合はschemaとtableそれぞれで保存処理
        if constexpr (std::is_same_v<std::decay_t<T>, nlohmann::json>)
        {
            std::vector<std::string> schema = results["schema"];
            std::vector<std::vector<std::string>> table = results["table"];
            writeComputationResult(job_uuid, schema, true);
            writeComputationResult(job_uuid, table);
            return;
        }

        std::string data_name = "";
        int column_number = -1;
        std::vector<std::string> result_list;
        if constexpr (std::is_same_v<std::decay_t<T>, std::vector<std::string>>)
        {
            data_name = "dim1";
            column_number = results.size();
            result_list.reserve(results.size());
            result_list = results;
        }
        else if constexpr (std::is_same_v<std::decay_t<T>, std::vector<std::vector<std::string>>>)
        {
            // ２次元配列の場合は1次元に変換する
            data_name = "dim2";
            column_number = (results.empty() ? -1 : results[0].size());
            size_t size = (results.empty() ? 0 : results.size() * results[0].size());
            result_list.reserve(size);
            for (auto row : results)
            {
                for (auto val : row)
                {
                    result_list.push_back(val);
                }
            }
        }
        else
        {
            qmpc::Log::throw_with_trace(std::runtime_error(
                "data type must be `std::vector<std::string>>` or `std::vector<std::string>>`"
            ));
        }
        if (is_schema)
        {
            data_name = "schema";
        }
        if (column_number <= 0)
        {
            // NOTE: Client側で復元する際に0以下だと不都合が生じるため
            column_number = 1;
        }

        // 巨大なデータはpieceに分割して保存する
        std::vector<std::string> piece_result;
        piece_result.reserve(piece_size);
        int piece_id = 0;
        int current_size = 0;
        auto add_piece = [&]()
        {
            nlohmann::json piece_data_json = {
                {"job_uuid", job_uuid},
                {"result", piece_result},
                {"meta", {{"piece_id", piece_id}, {"column_number", column_number}}}};
            const std::string data = piece_data_json.dump();

            auto ofs = std::ofstream(
                resultDbPath + job_uuid + "/" + data_name + "_" + std::to_string(piece_id)
            );
            ofs << data;
            ofs.close();

            current_size = 0;
            ++piece_id;
            piece_result.clear();
        };

        for (const auto &val : result_list)
        {
            int size = val.size();
            // resultのsizeがpieceの最大サイズを超える場合は一旦保存
            if (current_size + size >= piece_size)
            {
                add_piece();
            }
            piece_result.emplace_back(val);
            current_size += size;
        }
        add_piece();

        std::ofstream(resultDbPath + job_uuid + "/completed");
    }
};
}  // namespace qmpc::ComputationToDb
