#include "Client.hpp"

#include <chrono>
#include <experimental/filesystem>
#include <fstream>
#include <regex>

#include <google/protobuf/util/json_util.h>

namespace qmpc::ComputationToDb
{

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
    std::map<int, nlohmann::json> pieces;
    int all_size = 0;
    std::vector<std::string> schemas;
    for (const auto &entry : fs::directory_iterator(shareDbPath + data_id))
    {
        auto ifs = std::ifstream(entry.path());
        std::string data;
        getline(ifs, data);
        auto json = nlohmann::json::parse(data);

        all_size += json["value"].size();
        auto piece_id = json["meta"]["piece_id"];
        pieces.emplace(piece_id, json["value"]);

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

// model parameterの取り出し
std::vector<std::string> Client::readModelparam(const std::string &job_uuid) const
{
    // DBから値を取り出す
    int all_size = 0;
    std::map<int, std::vector<std::string>> pieces;
    std::regex is_status_file(R"((.*/completed)|(.*/status_.*))");
    for (const auto &entry : fs::directory_iterator(resultDbPath + job_uuid))
    {
        if (std::regex_match(entry.path().string(), is_status_file)) continue;

        auto ifs = std::ifstream(entry.path());
        std::string data;
        getline(ifs, data);

        auto json = nlohmann::json::parse(data);
        std::vector<std::string> result_piece = json["result"];
        all_size += result_piece.size();
        pieces.emplace(json["meta"]["piece_id"], result_piece);
    }

    // piece_id順にresultを結合
    std::vector<std::string> result;
    result.reserve(all_size);
    for (const auto &[_, piece] : pieces)
    {
        static_cast<void>(_);
        for (const auto &s : piece)
        {
            result.emplace_back(s);
        }
    }
    return result;
}

// XXX: Jsonは対応していないので削除する
// model parameter(json)の取り出し
nlohmann::json Client::readModelparamJson(const std::string &job_uuid) const
{
    return nlohmann::json();
}

// Job を DB に新規登録する
void Client::registerJob(const std::string &job_uuid, const int &status) const
{
    fs::create_directories(resultDbPath + job_uuid);
    updateJobStatus(job_uuid, status);
}

// Job の実行状態を更新する
void Client::updateJobStatus(const std::string &job_uuid, const int &status) const
{
    const google::protobuf::EnumDescriptor *descriptor =
        google::protobuf::GetEnumDescriptor<pb_common_types::JobStatus>();

    std::ofstream ofs(
        resultDbPath + job_uuid + "/status_" + descriptor->FindValueByNumber(status)->name()
    );

    std::chrono::system_clock::time_point tp = std::chrono::system_clock::now();
    auto tp_msec = std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch());
    ofs << tp_msec.count();
}

void Client::saveErrorInfo(const std::string &job_uuid, const pb_common_types::JobErrorInfo &info)
    const
{
    const google::protobuf::EnumDescriptor *descriptor =
        google::protobuf::GetEnumDescriptor<pb_common_types::JobStatus>();

    std::ofstream ofs(
        resultDbPath + job_uuid + "/status_"
        + descriptor->FindValueByNumber(pb_common_types::JobStatus::ERROR)->name()
    );

    static const auto options = []()
    {
        google::protobuf::util::JsonPrintOptions options;
        options.add_whitespace = true;
        options.always_print_enums_as_ints = false;
        options.always_print_primitive_fields = false;
        options.preserve_proto_field_names = true;
        return options;
    }();

    std::string dst;
    google::protobuf::util::MessageToJsonString(info, &dst, options);

    ofs << dst;
}

// tableデータを結合して取り出す
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

}  // namespace qmpc::ComputationToDb
