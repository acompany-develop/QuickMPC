#include "client.hpp"

#include <boost/format/format_fwd.hpp>
#include <chrono>
#include <experimental/filesystem>
#include <fstream>
#include <regex>
#include <stdexcept>

#include <google/protobuf/util/json_util.h>

namespace qmpc::ComputationToDb
{

nlohmann::json convertSchemaToJson(const qmpc::ComputationToDb::SchemaType &src)
{
    pb_common_types::Schema col_pb;
    col_pb.set_name(std::get<0>(src));
    col_pb.set_type(std::get<1>(src));
    std::string json_str;
    const google::protobuf::util::Status status =
        google::protobuf::util::MessageToJsonString(col_pb, &json_str);
    if (!status.ok())
    {
        qmpc::Log::throw_with_trace(
            std::invalid_argument((boost::format("%s") % status.message()).str())
        );
    }
    return nlohmann::json::parse(json_str);
}

std::vector<nlohmann::json> convertSchemaVectorToJsonVector(
    const std::vector<qmpc::ComputationToDb::SchemaType> &src
)
{
    std::vector<nlohmann::json> dst;
    for (const qmpc::ComputationToDb::SchemaType &col : src)
    {
        auto json = convertSchemaToJson(col);
        dst.emplace_back(json);
    }
    return dst;
}

/************ ComputationResultWriter ************/
ComputationResultWriter::ComputationResultWriter(
    const std::string &job_uuid, int data_type, int column_number, int piece_size
)
    : current_size(0)
    , piece_id(0)
    , job_uuid(job_uuid)
    , data_type(data_type)
    // NOTE: Client側で復元する際に0以下だと不都合が生じるため
    , column_number(std::max(1, column_number))
    , piece_size(piece_size)
{
}

void ComputationResultWriter::write(bool fin)
{
    nlohmann::json piece_data_json = {
        {"job_uuid", job_uuid},
        {"result", piece_data},
        {"meta", {{"piece_id", piece_id}, {"column_number", column_number}}}};
    const std::string data = piece_data_json.dump();
    Client::getInstance()->writeResultDB(job_uuid, data, data_type, piece_id);

    ++piece_id;
    current_size = 0;
    piece_data.clear();

    if (fin)
    {
        Client::getInstance()->updateJobCompleted(job_uuid);
    }
}

void ComputationResultWriter::emplace(const std::string &s)
{
    int size = s.size();
    if (current_size + size >= piece_size)
    {
        write(false);
    }
    piece_data.emplace_back(s);
    current_size += size;
}
void ComputationResultWriter::emplace(const std::vector<std::string> &v)
{
    for (const auto &x : v)
    {
        emplace(x);
    }
}

void ComputationResultWriter::emplace(const std::vector<std::vector<std::string>> &v)
{
    for (const auto &x : v)
    {
        emplace(x);
    }
}

void ComputationResultWriter::emplace(const SchemaType &s)
{
    auto json = convertSchemaToJson(s);
    emplace(json.dump());
}

void ComputationResultWriter::emplace(const std::vector<SchemaType> &v)
{
    for (const auto &x : v)
    {
        emplace(x);
    }
}

/************ TableWriter ************/
TableWriter::TableWriter(const std::string &data_id, int piece_size)
    : current_size(0), piece_id(0), data_id(data_id), piece_size(piece_size)
{
}

void TableWriter::write()
{
    // データを書き込む
    nlohmann::json piece_data_json = {
        {"value", piece_data}, {"meta", {{"piece_id", piece_id}, {"schema", json_schemas}}}};
    const std::string data = piece_data_json.dump();
    Client::getInstance()->writeShareDB(data_id, data, piece_id);

    // 書き込んだデータをclearしてpieceを進める
    ++piece_id;
    current_size = 0;
    piece_data.clear();
    json_schemas.clear();
}

void TableWriter::emplace(const std::vector<std::string> &v)
{
    int size = 0;
    for (const auto &s : v)
    {
        size += s.size();
    }
    if (current_size + size > piece_size)
    {
        write();
    }
    piece_data.emplace_back(v);
    current_size += size;
}

void TableWriter::emplace(const std::vector<SchemaType> &s)
{
    json_schemas = convertSchemaVectorToJsonVector(s);
}

/************ Client ************/

static std::vector<SchemaType> load_schema(const nlohmann::json &json)
{
    if (!json.is_array())
    {
        qmpc::Log::throw_with_trace(std::invalid_argument("schema is not array"));
    }
    std::vector<SchemaType> schema;
    for (const auto &elem : json)
    {
        if (elem.is_object())
        {
            pb_common_types::Schema column;
            const google::protobuf::util::Status status =
                google::protobuf::util::JsonStringToMessage(elem.dump(), &column);
            if (!status.ok())
            {
                qmpc::Log::throw_with_trace(
                    std::invalid_argument((boost::format("object is not represented by %s: %s")
                                           % column.GetDescriptor()->full_name() % status.message())
                                              .str())
                );
            }
            schema.emplace_back(column.name(), column.type());
        }
        else if (elem.is_string())
        {
            schema.emplace_back(
                elem.get<std::string>(),
                pb_common_types::ShareValueTypeEnum::SHARE_VALUE_TYPE_FIXED_POINT
            );
        }
        else
        {
            qmpc::Log::throw_with_trace(std::invalid_argument(
                (boost::format("schema type: %s is not supported") % elem.type_name()).str()
            ));
        }
    }
    return schema;
}

Client::Client() {}

std::shared_ptr<Client> Client::getInstance()
{
    static auto instance = std::make_shared<Client>();
    return instance;
}

// Tableの取り出し
std::optional<std::vector<std::vector<std::string>>> Client::readTable(
    const std::string &data_id, int piece_id
) const
{
    auto data_path = shareDbPath / data_id / std::to_string(piece_id);
    if (!fs::exists(data_path))
    {
        return std::nullopt;
    }

    auto ifs = std::ifstream(data_path);
    std::string data;
    getline(ifs, data);
    auto data_json = nlohmann::json::parse(data);

    std::vector<std::vector<std::string>> table;
    for (const auto &row : data_json["value"])
    {
        table.emplace_back(row);
    }
    return table;
}

// Schemaの取り出し
std::vector<SchemaType> Client::readSchema(const std::string &data_id) const
{
    // DBから値を取り出す
    auto ifs = std::ifstream(shareDbPath / data_id / "0");
    std::string data;
    getline(ifs, data);
    auto json = nlohmann::json::parse(data);
    auto j = json["meta"]["schema"];
    std::vector<SchemaType> schemas = load_schema(j);
    return schemas;
}

// shareDBに対してdataを書き込む
void Client::writeShareDB(const std::string &data_id, const std::string &data, int piece_id)
{
    fs::create_directories(shareDbPath / data_id);
    auto ofs = std::ofstream(shareDbPath / data_id / std::to_string(piece_id));
    ofs << data;
    ofs.close();
}

// resultDBに対してdataを書き込む
void Client::writeResultDB(
    const std::string &job_uuid, const std::string &data, int data_type, int piece_id
)
{
    fs::create_directories(resultDbPath / job_uuid);
    std::string data_name = (data_type == 0) ? "dim1" : (data_type == 1) ? "dim2" : "schema";
    auto data_file = data_name + "_" + std::to_string(piece_id);
    auto ofs = std::ofstream(shareDbPath / job_uuid / data_file);
    ofs << data;
    ofs.close();
}

// Job を DB に新規登録する
void Client::registerJob(const std::string &job_uuid, const int &status) const
{
    fs::create_directories(resultDbPath / job_uuid);
    updateJobStatus(job_uuid, status);
}

// Job の実行状態を更新する
void Client::updateJobStatus(const std::string &job_uuid, const int &status) const
{
    const google::protobuf::EnumDescriptor *descriptor =
        google::protobuf::GetEnumDescriptor<pb_common_types::JobStatus>();

    auto status_file = "status_" + descriptor->FindValueByNumber(status)->name();
    std::ofstream ofs(resultDbPath / job_uuid / status_file);

    std::chrono::system_clock::time_point tp = std::chrono::system_clock::now();
    auto tp_msec = std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch());
    ofs << tp_msec.count();
}

// Job の完了を登録する
void Client::updateJobCompleted(const std::string &job_uuid) const
{
    std::ofstream(resultDbPath / job_uuid / "completed");
}

void Client::saveErrorInfo(const std::string &job_uuid, const pb_common_types::JobErrorInfo &info)
    const
{
    const google::protobuf::EnumDescriptor *descriptor =
        google::protobuf::GetEnumDescriptor<pb_common_types::JobStatus>();

    auto status_file =
        "status_" + descriptor->FindValueByNumber(pb_common_types::JobStatus::ERROR)->name();
    std::ofstream ofs(resultDbPath / job_uuid / status_file);

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
