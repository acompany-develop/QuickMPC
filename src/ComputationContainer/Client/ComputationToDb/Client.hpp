#pragma once

#include <memory>
#include <string>
#include <utility>
#include <vector>

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
        const std::string &job_uuid, const T &results, bool is_schema=false, int piece_size = 1000000
    ) const
    {
        // resultsの形式は1次元 or 2次元 or json

        // NOTE: 現バージョンでresultsがjsonとなるのは結合リクエストのみ
        // 結合リクエストの場合はschemaとtableそれぞれで保存処理
        if constexpr(std::is_same_v<std::decay_t<T>, nlohmann::json>){
            std::vector<std::string> schema = results["schema"];
            std::vector<std::vector<std::string>> table = results["table"];
            writeComputationResult(job_uuid,schema,true);
            writeComputationResult(job_uuid,table);
            return;
        }

        int column_number = -1;
        std::vector<std::string> result_list;
        if constexpr(std::is_same_v<std::decay_t<T>, std::vector<std::string>>)
        {
            column_number = results.size();
            result_list.reserve(results.size());
            result_list = results;
        }
        else if constexpr(std::is_same_v<std::decay_t<T>, std::vector<std::vector<std::string>>>)
        {
            // ２次元配列の場合は1次元に変換する
            column_number = results[0].size();
            size_t size = results.size() * results[0].size();
            result_list.reserve(size);

            for (auto row : results){
                for (auto val : row){
                    result_list.push_back(val);
                }
            }
        }
        else
        {
            // TODO: throw
        }

        // 巨大なデータはpieceに分割して保存する
        int piece_id = 0;
        int tmp = 0;
        std::vector<std::string> piece_result;
        for (auto val : result_list){
            if (tmp+val.size()>=piece_size){
                nlohmann::json piece_data_json = {
                    {"job_uuid", job_uuid}, {"result", piece_result}, {"meta", {{"piece_id", piece_id},{"column_number", column_number}}}};
                const std::string data = piece_data_json.dump();

                auto ofs = std::ofstream(resultDbPath + job_uuid + "/" + (is_schema? "schema":"") + std::to_string(piece_id));
                ofs << data;
                ofs.close();

                tmp = 0;
                piece_id++;
                piece_result.clear();
            }
            piece_result.push_back(val);
            tmp += val.size();
        }
        nlohmann::json piece_data_json = {
            {"job_uuid", job_uuid}, {"result", piece_result}, {"meta", {{"piece_id", piece_id},{"column_number", column_number}}}};
        const std::string data = piece_data_json.dump();

        auto ofs = std::ofstream(resultDbPath + job_uuid + "/" + (is_schema? "schema":"") + std::to_string(piece_id));
        ofs << data;
        ofs.close();

        std::ofstream(resultDbPath + job_uuid + "/completed");
    }
};
}  // namespace qmpc::ComputationToDb
