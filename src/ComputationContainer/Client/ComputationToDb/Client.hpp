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

    void saveErrorInfo(const std::string &job_uuid, const pb_common_types::JobErrorInfo &info)
        const;

    // tableデータを結合して取り出す
    ValueTable readTable(const managetocomputation::JoinOrder &table);

    // resultの保存
    template <class T>
    void writeComputationResult(
        const std::string &job_uuid, const T &results, int piece_size = 1000000
    ) const
    {
        const nlohmann::json data_json = results;
        const std::string data_str = data_json.dump();

        // 巨大なデータはpieceに分割して保存する
        for (size_t left = 0; left < data_str.size(); left += piece_size)
        {
            const auto piece_str = data_str.substr(left, piece_size);
            const int piece_id = left / piece_size;

            nlohmann::json piece_data_json = {
                {"job_uuid", job_uuid}, {"result", piece_str}, {"meta", {{"piece_id", piece_id}}}};
            const std::string data = piece_data_json.dump();

            auto ofs = std::ofstream(resultDbPath + job_uuid + "/" + std::to_string(piece_id));
            ofs << data;
            ofs.close();
        }
        std::ofstream(resultDbPath + job_uuid + "/completed");
    }
};
}  // namespace qmpc::ComputationToDb
