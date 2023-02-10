#include "client.hpp"

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

// Tableの取り出し
std::vector<std::vector<std::string>> Client::readTable(const std::string &data_id) const
{
    // DBから値を取り出す
    std::map<int, nlohmann::json> pieces;
    int all_size = 0;
    for (const auto &entry : fs::directory_iterator(shareDbPath + data_id))
    {
        auto ifs = std::ifstream(entry.path());
        std::string data;
        getline(ifs, data);
        auto json = nlohmann::json::parse(data);

        all_size += json["value"].size();
        auto piece_id = json["meta"]["piece_id"];
        pieces.emplace(piece_id, json["value"]);
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
    return table;
}

// Schemaの取り出し
std::vector<std::string> Client::readSchema(const std::string &data_id) const
{
    // DBから値を取り出す
    std::vector<std::string> schemas;
    auto ifs = std::ifstream(shareDbPath + data_id + "/0");
    std::string data;
    getline(ifs, data);
    auto json = nlohmann::json::parse(data);
    auto j = json["meta"]["schema"];
    schemas = std::vector<std::string>(j.begin(), j.end());
    return schemas;
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
}  // namespace qmpc::ComputationToDb
