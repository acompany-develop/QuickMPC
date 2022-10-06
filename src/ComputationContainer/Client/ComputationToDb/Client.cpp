#include "Client.hpp"

#include <experimental/filesystem>
#include <fstream>
// XXX: 実装終わったら削除する
#include <iostream>
using std::cout;
using std::endl;

namespace qmpc::ComputationToDb
{

// TODO: ValueTableのファイル移動が完了したら削除する
using qmpc::ComputationToDbGate::ValueTable;

namespace fs = std::experimental::filesystem;

Client::Client() {}

std::shared_ptr<Client> Client::getInstance()
{
    static auto instance = std::make_shared<Client>();
    return instance;
}

// shareの取り出し
ValueTable Client::readShare(const std::string &data_id) const
{
    // DBから値を取り出す
    using PAIR = std::pair<int, const nlohmann::json &>;
    std::map<int, nlohmann::json> pieces;
    int all_size = 0;
    std::vector<std::string> schemas;
    for (const auto &entry : fs::directory_iterator(shareDbPath + data_id))
    {
        auto ifs = std::ifstream(entry.path());
        std::string data;
        ifs >> data;
        auto json = nlohmann::json::parse(data);

        all_size += json["value"].size();
        auto piece_id = json["meta"]["piece_id"];
        pieces.emplace(PAIR{piece_id, json["value"]});

        // schemaを1番目の要素だけから取り出す
        if (piece_id == 0)
        {
            auto j = json["meta"]["schema"];
            schemas = std::vector<std::string>(j.begin(), j.end());
        }
    }

    // piece_id順にvalueを結合
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
    return ValueTable(table, schemas);
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
ValueTable Client::readTable(const managetocomputation::JoinOrder &table)
{
    std::vector<std::vector<std::string>> t;
    std::vector<std::string> s;
    return ValueTable(t, s);
}

}  // namespace qmpc::ComputationToDb
