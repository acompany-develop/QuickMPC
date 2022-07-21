#pragma once

#include <string>
#include <vector>
#include <memory>
#include <optional>
#include <utility>

#include "external/Proto/ManageToComputationContainer/manage_to_computation.grpc.pb.h"

#include "Client/AnyToDb/N1QL.hpp"
#include "nlohmann/json.hpp"
#include "Client/AnyToDb/Client.hpp"
#include "Client/ComputationToDbGate/ValueTable.hpp"

namespace qmpc::ComputationToDbGate
{
class Client final
{
    const AnyToDb::Client client_share;

public:
    Client();
    static std::shared_ptr<Client> getInstance();

    // shareの取り出し
    ValueTable readShare(const std::string &) const;

    // model parameterの取り出し
    std::vector<std::string> readModelparam(const std::string &) const;
    nlohmann::json readModelparamJson(const std::string &) const;

    // resultの保存
    template <class T>
    void writeComputationResult(
        const std::string &job_uuid, const T &results, int piece_size = 1000000
    ) const
    {
        const nlohmann::json data_json = results;
        const std::string data_str = data_json.dump();

        const auto n1ql = AnyToDb::N1QL("result");
        const std::string query = n1ql.update(
            {"job_uuid", job_uuid},
            std::pair<std::string, std::string>{"result", data_str.substr(0, piece_size)}
        );
        client_share.executeQuery(query);

        // 巨大なデータはpieceに分割して保存する
        // 1-piece目以外はstatusが不要なので最低限でinsertする
        for (size_t left = piece_size; left < data_str.size(); left += piece_size)
        {
            const auto piece_str = data_str.substr(left, piece_size);
            const int piece_id = left / piece_size + 1;

            nlohmann::json piece_data_json;
            piece_data_json["job_uuid"] = job_uuid;
            piece_data_json["result"] = piece_str;
            nlohmann::json meta;
            meta["piece_id"] = piece_id;
            piece_data_json["meta"] = meta;
            const std::string piece_data_str = piece_data_json.dump();

            const auto values = AnyToDb::N1QLValue(piece_data_str);

            client_share.executeQuery(n1ql.insert(values));
        }
    }

    // Job を DB に新規登録する
    void registerJob(const std::string &job_uuid, const int &status) const;

    // Job の実行状態を更新する
    void updateJobStatus(const std::string &job_uuid, const int &status) const;

    // tableデータを結合して取り出す
    ValueTable readTable(const managetocomputation::JoinOrder &table);
};
}  // namespace qmpc::ComputationToDbGate
