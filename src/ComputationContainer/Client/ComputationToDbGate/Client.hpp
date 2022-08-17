#pragma once

#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "Client/AnyToDb/Client.hpp"
#include "Client/AnyToDb/N1QL.hpp"
#include "Client/ComputationToDbGate/ValueTable.hpp"
#include "external/Proto/ManageToComputationContainer/manage_to_computation.grpc.pb.h"
#include "nlohmann/json.hpp"

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
    void writeComputationResult(const std::string &job_uuid, const T &results) const
    {
        const nlohmann::json data_json = results;
        const std::string data_str = data_json.dump();

        const auto n1ql = AnyToDb::N1QL("result");
        const std::string query = n1ql.update({"job_uuid", job_uuid}, {"result", data_str});

        client_share.executeQuery(query);
    }

    // Job を DB に新規登録する
    void registerJob(const std::string &job_uuid, const int &status) const;

    // Job の実行状態を更新する
    void updateJobStatus(const std::string &job_uuid, const int &status) const;

    // tableデータを結合して取り出す
    ValueTable readTable(const managetocomputation::JoinOrder &table);
};
}  // namespace qmpc::ComputationToDbGate
