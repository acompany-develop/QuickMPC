#include <experimental/filesystem>
#include <fstream>

#include "client/computation_to_db/client.hpp"
#include "external/proto/common_types/common_types.pb.h"
#include "gtest/gtest.h"
namespace fs = std::experimental::filesystem;

// DBを初期化する
auto initialize(const std::string& id)
{
    fs::remove_all("/db/share/" + id);
    fs::remove_all("/db/result/" + id);
}

// tableの取り出し
// std::optional<std::vector<std::vector<std::string>>> readTable(const std::string &, int) const;
TEST(ComputationToDbTest, SuccessReadTableTest)
{
    const std::string data_id = "SuccessReadTableTest";
    initialize(data_id);

    const std::string data = R"({"value":[["1","2"],["3","4"]])"
                             R"(,"meta":{"piece_id":0,"schema":["attr1","attr2"]}})";
    fs::create_directories("/db/share/" + data_id);
    auto ofs = std::ofstream("/db/share/" + data_id + "/0");
    ofs << data;
    ofs.close();

    auto cc_to_db = qmpc::ComputationToDb::Client::getInstance();
    auto read_data = cc_to_db->readTable(data_id, 0).value();

    std::vector<std::vector<std::string>> true_table = {{"1", "2"}, {"3", "4"}};
    EXPECT_EQ(read_data, true_table);

    initialize(data_id);
}
TEST(ComputationToDbTest, SuccessReadTablePieceTest)
{
    const std::string data_id = "SuccessReadTablePieceTest";
    initialize(data_id);

    const std::vector<std::string> data = {
        R"({"value":[["1","2"],["3","4"]])"
        R"(,"meta":{"piece_id":0,"schema":["attr1","attr2"]}})",
        R"({"value":[["5","6"],["7","8"]])"
        R"(,"meta":{"piece_id":1,"schema":["attr1","attr2"]}})",
        R"({"value":[["9","10"]])"
        R"(,"meta":{"piece_id":2,"schema":["attr1","attr2"]}})"};
    fs::create_directories("/db/share/" + data_id);
    for (size_t piece_id = 0; piece_id < data.size(); ++piece_id)
    {
        auto ofs = std::ofstream("/db/share/" + data_id + "/" + std::to_string(piece_id));
        ofs << data[piece_id];
        ofs.close();
    }

    auto cc_to_db = qmpc::ComputationToDb::Client::getInstance();
    std::vector<std::vector<std::string>> read_data;
    for (size_t piece_id = 0; piece_id < data.size(); ++piece_id)
    {
        auto piece = cc_to_db->readTable(data_id, piece_id);
        for (const auto& row : piece.value())
        {
            read_data.emplace_back(row);
        }
    }

    std::vector<std::vector<std::string>> true_table = {
        {"1", "2"}, {"3", "4"}, {"5", "6"}, {"7", "8"}, {"9", "10"}};
    EXPECT_EQ(read_data, true_table);

    initialize(data_id);
}
TEST(ComputationToDbTest, SuccessReadEmptyTableTest)
{
    const std::string data_id = "SuccessReadEmptyTableTest";
    initialize(data_id);

    const std::string data = R"({"value":[])"
                             R"(,"meta":{"piece_id":0,"schema":[]}})";
    fs::create_directories("/db/share/" + data_id);
    auto ofs = std::ofstream("/db/share/" + data_id + "/0");
    ofs << data;
    ofs.close();

    auto cc_to_db = qmpc::ComputationToDb::Client::getInstance();
    auto read_data = cc_to_db->readTable(data_id, 0).value();

    std::vector<std::vector<std::string>> true_table = {};
    EXPECT_EQ(read_data, true_table);

    initialize(data_id);
}
TEST(ComputationToDbTest, SuccessReadShareWithSchemaTest)
{
    const std::string data_id = "SuccessReadShareWithSchemaTest";
    initialize(data_id);

    const std::string data = R"({"value": [["1", "2"], ["3", "4"]],)"
                             R"("meta": {"piece_id": 0,"schema": [)"
                             R"({"name": "attr1", "type": 0},)"
                             R"({"name": "attr2", "type": 0}]}})";

    fs::create_directories("/db/share/" + data_id);
    auto ofs = std::ofstream("/db/share/" + data_id + "/0");
    ofs << data;
    ofs.close();

    std::vector<std::vector<std::string>> true_table = {{"1", "2"}, {"3", "4"}};
    using SchemaType = qmpc::ComputationToDb::SchemaType;
    std::vector<SchemaType> true_schema = {
        SchemaType("attr1", pb_common_types::ShareValueTypeEnum::SHARE_VALUE_TYPE_UNSPECIFIED),
        SchemaType("attr2", pb_common_types::ShareValueTypeEnum::SHARE_VALUE_TYPE_UNSPECIFIED)};

    auto cc_to_db = qmpc::ComputationToDb::Client::getInstance();
    EXPECT_EQ(true_table, cc_to_db->readTable(data_id, 0));
    EXPECT_EQ(true_schema, cc_to_db->readSchema(data_id));

    initialize(data_id);
}

TEST(ComputationToDbTest, SuccessReadShareLargeTest)
{
    std::string data_id = "SuccessReadTableLargeTest";
    initialize(data_id);

    constexpr int H = 500;
    constexpr int W = 500;
    using SchemaType = qmpc::ComputationToDb::SchemaType;
    std::vector<SchemaType> schema;
    for (int i = 0; i < W; i++)
        schema.emplace_back(SchemaType(
            "attr" + std::to_string(i + 1),
            pb_common_types::ShareValueTypeEnum::SHARE_VALUE_TYPE_FIXED_POINT
        ));
    std::vector<std::string> data;
    for (int i = 0; i < W; ++i) data.emplace_back(std::to_string(i + 1));

    std::vector<std::string> schema_str;
    for (const SchemaType& s : schema) schema_str.emplace_back(std::get<0>(s));

    fs::create_directories("/db/share/" + data_id);
    for (int piece_id = 0; piece_id < H; ++piece_id)
    {
        nlohmann::json data_json = {
            {"data_id", data_id},
            {"value", {data}},
            {"meta", {{"piece_id", piece_id}, {"schema", schema_str}}}};
        auto data_str = data_json.dump();

        auto ofs = std::ofstream("/db/share/" + data_id + "/" + std::to_string(piece_id));
        ofs << data_str;
        ofs.close();
    }

    auto cc_to_db = qmpc::ComputationToDb::Client::getInstance();
    std::vector<std::vector<std::string>> read_data;
    for (int piece_id = 0; piece_id < H; ++piece_id)
    {
        auto piece = cc_to_db->readTable(data_id, piece_id);
        for (const auto& row : piece.value())
        {
            read_data.emplace_back(row);
        }
    }

    std::vector<std::vector<std::string>> true_data;
    for (int i = 0; i < H; i++) true_data.push_back(data);
    EXPECT_EQ(read_data, true_data);

    initialize(data_id);
}

// schemaの取り出し
// std::vector<std::string> Client::readSchema(const std::string &data_id);
TEST(ComputationToDbTest, SuccessReadSchemaTest)
{
    const std::string data_id = "SuccessReadSchemaTest";
    initialize(data_id);

    const std::string data = R"({"value":[["1","2"],["3","4"]])"
                             R"(,"meta":{"piece_id":0,"schema":[)"
                             R"({"name": "attr1", "type": 0},)"
                             R"({"name": "attr2", "type": 0})"
                             R"(]}})";
    fs::create_directories("/db/share/" + data_id);
    auto ofs = std::ofstream("/db/share/" + data_id + "/0");
    ofs << data;
    ofs.close();

    auto cc_to_db = qmpc::ComputationToDb::Client::getInstance();
    auto read_data = cc_to_db->readSchema(data_id);

    using SchemaType = qmpc::ComputationToDb::SchemaType;
    std::vector<SchemaType> true_schema = {
        SchemaType("attr1", pb_common_types::ShareValueTypeEnum::SHARE_VALUE_TYPE_UNSPECIFIED),
        SchemaType("attr2", pb_common_types::ShareValueTypeEnum::SHARE_VALUE_TYPE_UNSPECIFIED)};
    EXPECT_EQ(true_schema, read_data);

    initialize(data_id);
}

// matching_columnの取り出し
// int Client::readMatchingColumn(const std::string &data_id);
TEST(ComputationToDbTest, SuccessReadMatchingColumn)
{
    const std::string data_id = "SuccessReadMatchingColumn";
    initialize(data_id);

    const std::string data = R"({"meta":{"matching_column":1}})";
    fs::create_directories("/db/share/" + data_id);
    auto ofs = std::ofstream("/db/share/" + data_id + "/0");
    ofs << data;
    ofs.close();

    auto cc_to_db = qmpc::ComputationToDb::Client::getInstance();
    auto matching_column = cc_to_db->readMatchingColumn(data_id);

    using SchemaType = qmpc::ComputationToDb::SchemaType;
    auto expected = 1;
    EXPECT_EQ(expected, matching_column);

    initialize(data_id);
}

// Job を DB に新規登録する
// void Client::registerJob(const std::string &job_uuid, const int &status);
TEST(ComputationToDbTest, SuccessRregisterJobTest)
{
    const std::string job_uuid = "SuccessRregisterJobTest";
    initialize(job_uuid);

    auto cc_to_db = qmpc::ComputationToDb::Client::getInstance();
    cc_to_db->registerJob(job_uuid, pb_common_types::JobStatus::UNKNOWN);

    auto exist = fs::exists("/db/result/" + job_uuid + "/status_UNKNOWN");
    EXPECT_TRUE(exist);

    initialize(job_uuid);
}

// Job の実行状態を更新する
// void Client::updateJobStatus(const std::string &job_uuid, const int &status);
TEST(ComputationToDbTest, SuccessupdateJobStatusTest)
{
    const std::string job_uuid = "SuccessupdateJobStatusTest";
    initialize(job_uuid);

    fs::create_directories("/db/result/" + job_uuid);
    const google::protobuf::EnumDescriptor* descriptor =
        google::protobuf::GetEnumDescriptor<pb_common_types::JobStatus>();
    for (int status = 0;; ++status)
    {
        auto ptr = descriptor->FindValueByNumber(status);
        if (ptr == nullptr) break;

        auto cc_to_db = qmpc::ComputationToDb::Client::getInstance();
        cc_to_db->updateJobStatus(job_uuid, status);

        auto exist = fs::exists("/db/result/" + job_uuid + "/status_" + ptr->name());
        EXPECT_TRUE(exist);
    }

    initialize(job_uuid);
}

TEST(ComputationToDbTest, SuccessComputationResultWriterArrayTest)
{
    const std::string job_uuid = "SuccessComputationResultWriterArrayTest";
    initialize(job_uuid);

    const std::vector<std::string> data = {"12", "15", "21"};
    fs::create_directories("/db/result/" + job_uuid);

    auto writer = qmpc::ComputationToDb::ComputationResultWriter(job_uuid, 0, 3);
    writer.emplace(data);
    writer.completed();

    auto ifs = std::ifstream("/db/result/" + job_uuid + "/dim1_0");
    std::string read_data;
    getline(ifs, read_data);
    const auto true_data = R"({"job_uuid":"SuccessComputationResultWriterArrayTest")"
                           R"(,"meta":{"column_number":3,"piece_id":0})"
                           R"(,"result":["12","15","21"]})";
    EXPECT_EQ(read_data, true_data);

    initialize(job_uuid);
}
TEST(ComputationToDbTest, SuccessComputationResultWriter2dimTest)
{
    const std::string job_uuid = "SuccessComputationResultWriter2dimTest";
    initialize(job_uuid);

    const std::vector<std::vector<std::string>> data = {{"12", "15"}, {"21", "51"}};
    fs::create_directories("/db/result/" + job_uuid);

    auto writer = qmpc::ComputationToDb::ComputationResultWriter(job_uuid, 1, 2);
    writer.emplace(data);
    writer.completed();

    auto ifs = std::ifstream("/db/result/" + job_uuid + "/dim2_0");
    std::string read_data;
    getline(ifs, read_data);
    const auto true_data = R"({"job_uuid":"SuccessComputationResultWriter2dimTest")"
                           R"(,"meta":{"column_number":2,"piece_id":0})"
                           R"(,"result":["12","15","21","51"]})";
    EXPECT_EQ(read_data, true_data);

    initialize(job_uuid);
}

TEST(ComputationToDbTest, SuccessComputationResultWriterSchemaTest)
{
    const std::string job_uuid = "SuccessComputationResultWriterSchemaTest";
    initialize(job_uuid);

    using SchemaType = qmpc::ComputationToDb::SchemaType;
    std::vector<SchemaType> schema = {
        SchemaType("s1", pb_common_types::ShareValueTypeEnum::SHARE_VALUE_TYPE_FIXED_POINT),
        SchemaType("s2", pb_common_types::ShareValueTypeEnum::SHARE_VALUE_TYPE_FIXED_POINT),
        SchemaType("s3", pb_common_types::ShareValueTypeEnum::SHARE_VALUE_TYPE_FIXED_POINT)};
    fs::create_directories("/db/result/" + job_uuid);

    auto writer = qmpc::ComputationToDb::ComputationResultWriter(job_uuid, 2, 3);
    writer.emplace(schema);
    writer.completed();

    auto ifs = std::ifstream("/db/result/" + job_uuid + "/schema_0");
    std::string read_data;
    getline(ifs, read_data);
    const auto true_data =
        R"({"job_uuid":"SuccessComputationResultWriterSchemaTest")"
        R"(,"meta":{"column_number":3,"piece_id":0})"
        R"(,"result":["{\"name\":\"s1\",\"type\":\"SHARE_VALUE_TYPE_FIXED_POINT\"}",)"
        R"("{\"name\":\"s2\",\"type\":\"SHARE_VALUE_TYPE_FIXED_POINT\"}","{\"name\":\"s3\",\"type\":\"SHARE_VALUE_TYPE_FIXED_POINT\"}"]})";
    EXPECT_EQ(read_data, true_data);

    initialize(job_uuid);
}

TEST(ComputationToDbTest, SuccessComputationResultWriterArrayPieceTest)
{
    const std::string job_uuid = "SuccessComputationResultWriterArrayPieceTest";
    initialize(job_uuid);

    const std::vector<std::vector<std::string>> data = {{"12", "15", "21"}};
    fs::create_directories("/db/result/" + job_uuid);

    auto writer = qmpc::ComputationToDb::ComputationResultWriter(job_uuid, 1, 3, 4);
    writer.emplace(data);
    writer.completed();

    std::vector<std::string> true_result = {R"(["12"])", R"(["15"])", R"(["21"])"};
    for (size_t piece_id = 0; piece_id < true_result.size(); ++piece_id)
    {
        auto ifs = std::ifstream("/db/result/" + job_uuid + "/dim2_" + std::to_string(piece_id));
        std::string read_data;
        getline(ifs, read_data);
        const auto true_data = R"({"job_uuid":"SuccessComputationResultWriterArrayPieceTest")"
                               R"(,"meta":{"column_number":3,"piece_id":)"
                               + std::to_string(piece_id) + R"(},"result":)" + true_result[piece_id]
                               + R"(})";
        EXPECT_EQ(read_data, true_data);
    }

    initialize(job_uuid);
}

TEST(ComputationToDbTest, SuccessComputationResultWriterCompletedTest)
{
    const std::string job_uuid = "SuccessComputationResultWriterCompletedTest";
    initialize(job_uuid);

    const std::vector<std::vector<std::string>> data = {{"12", "15", "21"}};
    fs::create_directories("/db/result/" + job_uuid);

    auto writer = qmpc::ComputationToDb::ComputationResultWriter(job_uuid, 1, 3);
    writer.emplace(data);
    writer.completed();

    auto exist = fs::exists("/db/result/" + job_uuid + "/completed");
    EXPECT_TRUE(exist);

    initialize(job_uuid);
}

TEST(ComputationToDbTest, SuccessTableWriteTest)
{
    const std::string data_id = "SuccessTableWriteTest";
    initialize(data_id);

    std::vector<std::vector<std::string>> table = {{"1", "2"}, {"3", "4"}};
    using SchemaType = qmpc::ComputationToDb::SchemaType;
    std::vector<SchemaType> schema = {
        SchemaType("attr1", pb_common_types::ShareValueTypeEnum::SHARE_VALUE_TYPE_FIXED_POINT),
        SchemaType("attr2", pb_common_types::ShareValueTypeEnum::SHARE_VALUE_TYPE_FIXED_POINT)};

    qmpc::ComputationToDb::TableWriter writer(data_id);

    writer.emplace(schema);
    for (const auto& row : table)
    {
        writer.emplace(row);
    }
    writer.write();

    auto ifs = std::ifstream("/db/share/" + data_id + "/0");
    std::string data;
    getline(ifs, data);
    std::string true_data =
        R"({"meta":{"piece_id":0,"schema":[{"name":"attr1","type":"SHARE_VALUE_TYPE_FIXED_POINT"},)"
        R"({"name":"attr2","type":"SHARE_VALUE_TYPE_FIXED_POINT"}]},"value":[["1","2"],["3","4"]]})";
    EXPECT_EQ(true_data, data);
    initialize(data_id);
}

TEST(ComputationToDbTest, SuccessTableWritePieceTest)
{
    const std::string data_id = "SuccessTableWriteTest";
    initialize(data_id);

    std::vector<std::vector<std::string>> table = {{"1", "2"}, {"3", "4"}, {"5", "6"}};
    using SchemaType = qmpc::ComputationToDb::SchemaType;
    std::vector<SchemaType> schema = {
        SchemaType("attr1", pb_common_types::ShareValueTypeEnum::SHARE_VALUE_TYPE_FIXED_POINT),
        SchemaType("attr2", pb_common_types::ShareValueTypeEnum::SHARE_VALUE_TYPE_FIXED_POINT)};

    qmpc::ComputationToDb::TableWriter writer(data_id, 4);

    writer.emplace(schema);
    for (const auto& row : table)
    {
        writer.emplace(row);
    }
    writer.write();

    std::vector<std::string> true_data = {
        R"({"meta":{"piece_id":0,"schema":[{"name":"attr1","type":"SHARE_VALUE_TYPE_FIXED_POINT"},)"
        R"({"name":"attr2","type":"SHARE_VALUE_TYPE_FIXED_POINT"}]},"value":[["1","2"],["3","4"]]})",

        R"({"meta":{"piece_id":1,"schema":[]},"value":[["5","6"]]})"};
    for (int piece_id = 0; piece_id < 2; ++piece_id)
    {
        auto ifs = std::ifstream("/db/share/" + data_id + "/" + std::to_string(piece_id));
        std::string data;
        getline(ifs, data);
        EXPECT_EQ(true_data[piece_id], data);
    }
    initialize(data_id);
}
