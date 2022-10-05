#include "Client.hpp"

namespace qmpc::ComputationToDb
{
Client::Client() {}

std::shared_ptr<Client> Client::getInstance()
{
    static auto instance = std::make_shared<Client>();
    return instance;
}

// shareの取り出し
qmpc::ComputationToDbGate::ValueTable Client::readShare(const std::string &data_id) const
{
    std::vector<std::vector<std::string>> t;
    std::vector<std::string> s;
    return qmpc::ComputationToDbGate::ValueTable(t, s);
}

// model parameter(vector)の取り出し
std::vector<std::string> Client::readModelparam(const std::string &job_uuid) const { return {}; }

// model parameter(json)の取り出し
nlohmann::json Client::readModelparamJson(const std::string &job_uuid) const { return {}; }

// Job を DB に新規登録する
void Client::registerJob(const std::string &job_uuid, const int &status) const {}

// Job の実行状態を更新する
void Client::updateJobStatus(const std::string &job_uuid, const int &status) const {}

// tableデータを結合して取り出す
qmpc::ComputationToDbGate::ValueTable Client::readTable(const managetocomputation::JoinOrder &table)
{
    std::vector<std::vector<std::string>> t;
    std::vector<std::string> s;
    return qmpc::ComputationToDbGate::ValueTable(t, s);
}

}  // namespace qmpc::ComputationToDb
