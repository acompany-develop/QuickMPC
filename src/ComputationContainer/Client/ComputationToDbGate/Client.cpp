#include "Client.hpp"

#include <map>
#include <chrono>
#include <thread>

namespace qmpc::ComputationToDbGate
{

Client::Client() : client_share("sharedb") {}

std::shared_ptr<Client> Client::getInstance()
{
    static auto instance = std::make_shared<Client>();
    return instance;
}

ValueTable Client::readShare(const std::string &data_id) const
{
    auto n1ql = AnyToDb::N1QL("share");
    auto response = client_share.executeQuery(n1ql.select_id("data_id", data_id));
    auto response_json = nlohmann::json::parse(response);

    // piece_id順にvalueを結合
    std::map<int, nlohmann::json> pieces;
    int all_size = 0;
    using PAIR = std::pair<int, const nlohmann::json &>;
    for (const auto &json : response_json)
    {
        all_size += json["value"].size();
        pieces.emplace(PAIR{json["meta"]["piece_id"], json["value"]});
    }
    std::vector<std::vector<std::string>> table;
    table.reserve(all_size);
    for (const auto &[_, piece] : pieces)
    {
        static_cast<void>(_);
        for (const auto &row : piece)
        {
            table.emplace_back(row);
        }
    }
    // schemaを取得
    auto schemas = response_json[0]["meta"]["schema"];
    return ValueTable(table, schemas);
}

// model parameterの取り出し
std::vector<std::string> Client::readModelparam(const std::string &job_uuid) const
{
    auto n1ql = AnyToDb::N1QL("result");
    auto response = client_share.executeQuery(n1ql.select_id("job_uuid", job_uuid));
    auto response_json = nlohmann::json::parse(response);
    std::vector<std::string> ret;
    ret.reserve(response_json[0]["result"].size());
    for (const auto &val : response_json[0]["result"])
    {
        ret.emplace_back(val);
    }
    return ret;
}
nlohmann::json Client::readModelparamJson(const std::string &job_uuid) const
{
    auto n1ql = AnyToDb::N1QL("result");
    auto response = client_share.executeQuery(n1ql.select_id("job_uuid", job_uuid));
    auto response_json = nlohmann::json::parse(response);
    return response_json[0]["result"];
}

void Client::registerJob(const std::string &job_uuid, const int &status) const
{
    nlohmann::json data_json;
    data_json["job_uuid"] = job_uuid;
    data_json["status"] = status;
    nlohmann::json meta;
    meta["piece_id"] = 1;
    data_json["meta"] = meta;
    const std::string data_str = data_json.dump();

    const auto values = AnyToDb::N1QLValue(data_str);

    auto n1ql = AnyToDb::N1QL("result");
    client_share.executeQuery(n1ql.insert(values));
}

void Client::updateJobStatus(const std::string &job_uuid, const int &status) const
{
    auto n1ql = AnyToDb::N1QL("result");
    const std::string query =
        n1ql.update({"job_uuid", job_uuid}, std::pair<std::string, int>{"status", status});

    client_share.executeQuery(query);
}

ValueTable Client::readTable(const managetocomputation::JoinOrder &table)
{
    // requestからデータ読み取り
    auto size = table.join().size();
    std::vector<int> join;
    join.reserve(size);
    for (const auto &j : table.join())
    {
        join.emplace_back(j);
    }
    std::vector<int> index;
    index.reserve(size);
    for (const auto &j : table.index())
    {
        index.emplace_back(j);
    }
    std::vector<ValueTable> tables;
    tables.reserve(size + 1);
    for (const auto &dataId : table.dataids())
    {
        tables.emplace_back(this->readShare(dataId));
    }
    return parseRead(tables, join, index);
}
}  // namespace qmpc::ComputationToDbGate
