#pragma once

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "Logging/Logger.hpp"
#include "ValueTable.hpp"
#include "external/Proto/ManageToComputationContainer/manage_to_computation.grpc.pb.h"
#include "external/Proto/common_types/common_types.pb.h"
#include "nlohmann/json.hpp"

namespace qmpc::ComputationToDb
{
class Client final
{
    static inline const std::string shareDbPath = "/Db/share/";
    static inline const std::string resultDbPath = "/Db/result/";

    std::string readModelparamString(const std::string &job_uuid) const;

public:
    Client();
    static std::shared_ptr<Client> getInstance();

    // shareの取り出し
    ValueTable readShare(const std::string &) const;

    // model parameterの取り出し
    std::vector<std::string> readModelparam(const std::string &) const;
    nlohmann::json readModelparamJson(const std::string &) const;

    // Job を DB に新規登録する
    void registerJob(const std::string &job_uuid, const int &status) const;

    // Job の実行状態を更新する
    void updateJobStatus(const std::string &job_uuid, const int &status) const;

    // Job 実行中に発生したエラーに関する情報を保存する
    void saveErrorInfo(const std::string &job_uuid, const pb_common_types::JobErrorInfo &info)
        const;

    // tableデータを結合して取り出す
    ValueTable readTable(const managetocomputation::JoinOrder &table);

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
            column_number = results[0].size();
            size_t size = results.size() * results[0].size();
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
            piece_result.emplace_back(val);
            current_size += val.size();
            if (current_size >= piece_size)
            {
                add_piece();
            }
        }
        if (!piece_result.empty())
        {
            add_piece();
        }

        std::ofstream(resultDbPath + job_uuid + "/completed");
    }
};
}  // namespace qmpc::ComputationToDb
