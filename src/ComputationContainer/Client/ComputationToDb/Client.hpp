#pragma once

#include <memory>
#include <string>
#include <utility>
#include <vector>

// DB移植が完了したらValueTableだけComputationToDb/に移動する
#include "Client/ComputationToDbGate/ValueTable.hpp"
#include "external/Proto/ManageToComputationContainer/manage_to_computation.grpc.pb.h"
#include "nlohmann/json.hpp"

namespace qmpc::ComputationToDb
{
class Client final
{
    static inline const std::string shareDbPath = "/Db/share/";
    static inline const std::string resultDbPath = "/Db/result/";

public:
    Client();
    static std::shared_ptr<Client> getInstance();

    // shareの取り出し
    qmpc::ComputationToDbGate::ValueTable readShare(const std::string &) const;

    // model parameterの取り出し
    std::vector<std::string> readModelparam(const std::string &) const;
    nlohmann::json readModelparamJson(const std::string &) const;

    // Job を DB に新規登録する
    void registerJob(const std::string &job_uuid, const int &status) const;

    // Job の実行状態を更新する
    void updateJobStatus(const std::string &job_uuid, const int &status) const;

    // tableデータを結合して取り出す
    qmpc::ComputationToDbGate::ValueTable readTable(const managetocomputation::JoinOrder &table);

    // resultの保存
    template <class T>
    void writeComputationResult(
        const std::string &job_uuid, const T &results, int piece_size = 1000000
    ) const
    {
    }
};
}  // namespace qmpc::ComputationToDb
