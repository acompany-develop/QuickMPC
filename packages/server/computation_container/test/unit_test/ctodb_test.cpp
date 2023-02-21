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
TEST(ComputationToDbTest, SuccessReadTableLargeTest)
{
    std::string data_id = "SuccessReadTableLargeTest";
    initialize(data_id);

    constexpr int H = 500;
    constexpr int W = 500;
    std::vector<std::string> schema;
    for (int i = 0; i < W; i++) schema.emplace_back("attr" + std::to_string(i + 1));
    std::vector<std::string> data;
    for (int i = 0; i < W; ++i) data.emplace_back(std::to_string(i + 1));

    fs::create_directories("/db/share/" + data_id);
    for (int piece_id = 0; piece_id < H; ++piece_id)
    {
        nlohmann::json data_json = {
            {"data_id", data_id},
            {"value", {data}},
            {"meta", {{"piece_id", piece_id}, {"schema", schema}}}};
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
                             R"(,"meta":{"piece_id":0,"schema":["attr1","attr2"]}})";
    fs::create_directories("/db/share/" + data_id);
    auto ofs = std::ofstream("/db/share/" + data_id + "/0");
    ofs << data;
    ofs.close();

    auto cc_to_db = qmpc::ComputationToDb::Client::getInstance();
    auto read_data = cc_to_db->readSchema(data_id);

    std::vector<std::string> true_schema = {"attr1", "attr2"};
    EXPECT_EQ(true_schema, read_data);

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

    auto exist = fs::exists("/db/result/" + job_uuid);
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

// resultの保存
// void writeComputationResult(const std::string &job_uuid, const T &result_list, int data_type, int
// column_number, int piece_size) const
TEST(ComputationToDbTest, SuccessWriteComputationResultArrayTest)
{
    const std::string job_uuid = "WriteComputaSuccessWriteComputationResultArrayTest";
    initialize(job_uuid);

    const std::vector<std::string> data = {"12", "15", "21"};
    fs::create_directories("/db/result/" + job_uuid);

    auto cc_to_db = qmpc::ComputationToDb::Client::getInstance();
    cc_to_db->writeComputationResult(job_uuid, data, 0, 3);

    auto ifs = std::ifstream("/db/result/" + job_uuid + "/dim1_0");
    std::string read_data;
    getline(ifs, read_data);
    const auto true_data = R"({"job_uuid":"WriteComputaSuccessWriteComputationResultArrayTest")"
                           R"(,"meta":{"column_number":3,"piece_id":0})"
                           R"(,"result":["12","15","21"]})";
    EXPECT_EQ(read_data, true_data);

    initialize(job_uuid);
}
TEST(ComputationToDbTest, SuccessWriteComputationResultArray2dimTest)
{
    const std::string job_uuid = "SuccessWriteComputationResultArray2dimTest";
    initialize(job_uuid);

    const std::vector<std::vector<std::string>> data = {{"12", "15"}, {"21", "51"}};
    fs::create_directories("/db/result/" + job_uuid);

    auto cc_to_db = qmpc::ComputationToDb::Client::getInstance();
    cc_to_db->writeComputationResult(job_uuid, data, 1, 2);

    auto ifs = std::ifstream("/db/result/" + job_uuid + "/dim2_0");
    std::string read_data;
    getline(ifs, read_data);
    const auto true_data = R"({"job_uuid":"SuccessWriteComputationResultArray2dimTest")"
                           R"(,"meta":{"column_number":2,"piece_id":0})"
                           R"(,"result":["12","15","21","51"]})";
    EXPECT_EQ(read_data, true_data);

    initialize(job_uuid);
}

TEST(ComputationToDbTest, SuccessWriteComputationResultSchemaTest)
{
    const std::string job_uuid = "SuccessWriteComputationResultSchemaTest";
    initialize(job_uuid);

    const std::vector<std::string> schema = {"s1", "s2", "s3"};
    fs::create_directories("/db/result/" + job_uuid);

    auto cc_to_db = qmpc::ComputationToDb::Client::getInstance();
    cc_to_db->writeComputationResult(job_uuid, schema, 2, 3);

    auto ifs = std::ifstream("/db/result/" + job_uuid + "/schema_0");
    std::string read_data;
    getline(ifs, read_data);
    const auto true_data = R"({"job_uuid":"SuccessWriteComputationResultSchemaTest")"
                           R"(,"meta":{"column_number":3,"piece_id":0})"
                           R"(,"result":["s1","s2","s3"]})";
    EXPECT_EQ(read_data, true_data);

    initialize(job_uuid);
}

TEST(ComputationToDbTest, SuccessWriteComputationResultArrayPieceTest)
{
    const std::string job_uuid = "SuccessWriteComputationResultArrayPieceTest";
    initialize(job_uuid);

    const std::vector<std::vector<std::string>> data = {{"12", "15", "21"}};
    fs::create_directories("/db/result/" + job_uuid);

    auto cc_to_db = qmpc::ComputationToDb::Client::getInstance();
    cc_to_db->writeComputationResult(job_uuid, data, 1, 3, 4);

    std::vector<std::string> true_result = {R"(["12"])", R"(["15"])", R"(["21"])"};
    for (size_t piece_id = 0; piece_id < true_result.size(); ++piece_id)
    {
        auto ifs = std::ifstream("/db/result/" + job_uuid + "/dim2_" + std::to_string(piece_id));
        std::string read_data;
        getline(ifs, read_data);
        const auto true_data = R"({"job_uuid":"SuccessWriteComputationResultArrayPieceTest")"
                               R"(,"meta":{"column_number":3,"piece_id":)"
                               + std::to_string(piece_id) + R"(},"result":)" + true_result[piece_id]
                               + R"(})";
        EXPECT_EQ(read_data, true_data);
    }

    initialize(job_uuid);
}

TEST(ComputationToDbTest, SuccessWriteComputationResultCompletedTest)
{
    const std::string job_uuid = "WriteComputationResultTestId";
    initialize(job_uuid);

    const std::vector<std::vector<std::string>> data = {{"12", "15", "21"}};
    fs::create_directories("/db/result/" + job_uuid);

    auto cc_to_db = qmpc::ComputationToDb::Client::getInstance();
    cc_to_db->writeComputationResult(job_uuid, data, 1, 3);

    auto exist = fs::exists("/db/result/" + job_uuid + "/completed");
    EXPECT_TRUE(exist);

    initialize(job_uuid);
}

TEST(ComputationToDbTest, SuccessWriteTableTest)
{
    const std::string data_id = "SuccessWriteTableTest";
    initialize(data_id);

    std::vector<std::vector<std::string>> table = {{"1", "2"}, {"3", "4"}};
    std::vector<std::string> schema = {"attr1", "attr2"};

    auto cc_to_db = qmpc::ComputationToDb::Client::getInstance();
    cc_to_db->writeTable(data_id, table, schema);

    auto ifs = std::ifstream("/db/share/" + data_id + "/0");
    std::string data;
    getline(ifs, data);

    std::string true_data = R"({"meta":{"piece_id":0,"schema":["attr1","attr2"]})"
                            R"(,"value":[["1","2"],["3","4"]]})";
    EXPECT_EQ(true_data, data);
}
