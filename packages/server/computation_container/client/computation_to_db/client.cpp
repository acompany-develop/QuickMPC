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

namespace fs = std::experimental::filesystem;

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
        auto json = convertSchemaToJson(src);
        dst.emplace_back(json);
    }
    return dst;
}

/************ Client::ComputationResultWriter ************/
Client::ComputationResultWriter::ComputationResultWriter(
    const std::string &job_uuid, int key, int column_number, int piece_size
)
    : current_size(0)
    , piece_id(0)
    , job_uuid(job_uuid)
    , data_name(
          (key == 0)   ? "dim1"
          : (key == 1) ? "dim2"
                       : "schema"
      )
    // NOTE: Client側で復元する際に0以下だと不都合が生じるため
    , column_number(std::max(1, column_number))
    , piece_size(piece_size)
{
}

void Client::ComputationResultWriter::write()
{
    nlohmann::json piece_data_json = {
        {"job_uuid", job_uuid},
        {"result", piece_data},
        {"meta", {{"piece_id", piece_id}, {"column_number", column_number}}}};
    const std::string data = piece_data_json.dump();

    auto ofs =
        std::ofstream(resultDbPath + job_uuid + "/" + data_name + "_" + std::to_string(piece_id));
    ofs << data;
    ofs.close();

    ++piece_id;
    current_size = 0;
    piece_data.clear();
}

void Client::ComputationResultWriter::emplace(const std::string &s)
{
    int size = s.size();
    if (current_size + size >= piece_size)
    {
        write();
    }
    piece_data.emplace_back(s);
    current_size += size;
}
void Client::ComputationResultWriter::emplace(const std::vector<std::string> &v)
{
    for (const auto &x : v)
    {
        emplace(x);
    }
}
void Client::ComputationResultWriter::emplace(const SchemaType &s)
{
    auto json = convertSchemaToJson(s);
    emplace(json.dump());
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
    auto data_path = shareDbPath + data_id + "/" + std::to_string(piece_id);
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
    auto ifs = std::ifstream(shareDbPath + data_id + "/0");
    std::string data;
    getline(ifs, data);
    auto json = nlohmann::json::parse(data);
    auto j = json["meta"]["schema"];
    std::vector<SchemaType> schemas = load_schema(j);
    return schemas;
}

// Tableの保存
std::string Client::writeTable(
    const std::string &data_id,
    std::vector<std::vector<std::string>> &table,
    const std::vector<SchemaType> &schema
) const
{
    // TODO: piece_idを引数に受け取ってpieceごとに保存できるようにする
    const int piece_id = 0;

    auto json_schema = convertSchemaVectorToJsonVector(schema);
    nlohmann::json data_json = {
        {"value", table}, {"meta", {{"piece_id", piece_id}, {"schema", json_schema}}}};
    const std::string data = data_json.dump();

    auto data_path = shareDbPath + data_id;
    fs::create_directories(data_path);
    std::ofstream ofs(data_path + "/" + std::to_string(piece_id));
    ofs << data;
    ofs.close();
    return data_id;
};

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
