#include <experimental/filesystem>
#include <fstream>

#include "Client/ComputationToDb/Client.hpp"
#include "external/Proto/common_types/common_types.pb.h"
#include "gtest/gtest.h"
namespace fs = std::experimental::filesystem;

// DBを初期化する
auto initialize(const std::string &id)
{
    fs::remove_all("/Db/share/" + id);
    fs::remove_all("/Db/result/" + id);
}

// shareの取り出し
// qmpc::ComputationToDbGate::ValueTable Client::readShare(const std::string &data_id);
TEST(ComputationToDbTest, SuccessReadShareTest)
{
    const std::string data_id = "ReadShareTestId";
    initialize(data_id);

    const std::string data = R"({"value":[["1","2"],["3","4"]])"
                             R"(,"meta":{"piece_id":0,"schema":["attr1","attr2"]}})";
    fs::create_directories("/Db/share/" + data_id);
    auto ofs = std::ofstream("/Db/share/" + data_id + "/0");
    ofs << data;
    ofs.close();

    auto cc_to_db = qmpc::ComputationToDb::Client::getInstance();
    auto read_data = cc_to_db->readShare(data_id);

    std::vector<std::vector<std::string>> true_table = {{"1", "2"}, {"3", "4"}};
    std::vector<std::string> true_schema = {"attr1", "attr2"};
    EXPECT_EQ(true_table, read_data.getTable());
    EXPECT_EQ(true_schema, read_data.getSchemas());

    initialize(data_id);
}
TEST(ComputationToDbTest, SuccessReadSharePieceTest)
{
    const std::string data_id = "ReadShareTestId";
    initialize(data_id);

    const std::vector<std::string> data = {
        R"({"value":[["1","2"],["3","4"]])"
        R"(,"meta":{"piece_id":0,"schema":["attr1","attr2"]}})",
        R"({"value":[["5","6"],["7","8"]])"
        R"(,"meta":{"piece_id":1,"schema":["attr1","attr2"]}})",
        R"({"value":[["9","10"]])"
        R"(,"meta":{"piece_id":2,"schema":["attr1","attr2"]}})"};
    fs::create_directories("/Db/share/" + data_id);
    for (size_t piece_id = 0; piece_id < data.size(); ++piece_id)
    {
        auto ofs = std::ofstream("/Db/share/" + data_id + "/" + std::to_string(piece_id));
        ofs << data[piece_id];
        ofs.close();
    }

    auto cc_to_db = qmpc::ComputationToDb::Client::getInstance();
    auto read_data = cc_to_db->readShare(data_id);

    std::vector<std::vector<std::string>> true_table = {
        {"1", "2"}, {"3", "4"}, {"5", "6"}, {"7", "8"}, {"9", "10"}};
    std::vector<std::string> true_schema = {"attr1", "attr2"};
    EXPECT_EQ(true_table, read_data.getTable());
    EXPECT_EQ(true_schema, read_data.getSchemas());

    initialize(data_id);
}
TEST(ComputationToDbTest, SuccessReadShareLargeTest)
{
    std::string data_id = "ReadShareTestId";
    initialize(data_id);

    constexpr int H = 500;
    constexpr int W = 500;
    std::vector<std::string> schema;
    for (int i = 0; i < W; i++) schema.emplace_back("attr" + std::to_string(i + 1));
    std::vector<std::string> data;
    for (int i = 0; i < W; ++i) data.emplace_back(std::to_string(i + 1));

    fs::create_directories("/Db/share/" + data_id);
    for (int piece_id = 0; piece_id < H; ++piece_id)
    {
        nlohmann::json data_json = {
            {"data_id", data_id},
            {"value", {data}},
            {"meta", {{"piece_id", piece_id}, {"schema", schema}}}};
        auto data_str = data_json.dump();

        auto ofs = std::ofstream("/Db/share/" + data_id + "/" + std::to_string(piece_id));
        ofs << data_str;
        ofs.close();
    }

    auto cc_to_db = qmpc::ComputationToDb::Client::getInstance();
    auto read_data = cc_to_db->readShare(data_id);

    std::vector<std::vector<std::string>> true_data;
    for (int i = 0; i < H; i++) true_data.push_back(data);
    EXPECT_EQ(true_data, read_data.getTable());
    EXPECT_EQ(schema, read_data.getSchemas());

    initialize(data_id);
}

// model parameter(vector)の取り出し
// std::vector<std::string> Client::readModelparam(const std::string &job_uuid);
TEST(ComputationToDbTest, SuccessReadModelParamTest)
{
    const std::string job_uuid = "ReadModelParamTestId";
    initialize(job_uuid);

    const std::string data = R"({"result":"[\"1\",\"2\",\"3\"]")"
                             R"(,"meta":{"piece_id":0}})";
    fs::create_directories("/Db/result/" + job_uuid);
    auto ofs = std::ofstream("/Db/result/" + job_uuid + "/0");
    ofs << data;
    ofs.close();

    auto cc_to_db = qmpc::ComputationToDb::Client::getInstance();
    auto read_data = cc_to_db->readModelparam(job_uuid);

    std::vector<std::string> true_result = {"1", "2", "3"};
    EXPECT_EQ(true_result, read_data);

    initialize(job_uuid);
}

TEST(ComputationToDbTest, SuccessReadModelParamPieceTest)
{
    const std::string job_uuid = "ReadModelParamTestId";
    initialize(job_uuid);

    const std::vector<std::string> data = {
        R"({"result":"[\"1\",\"2\"")"
        R"(,"meta":{"piece_id":0}})",
        R"({"result":",\"3\"")"
        R"(,"meta":{"piece_id":1}})",
        R"({"result":",\"4\"]")"
        R"(,"meta":{"piece_id":2}})"};
    fs::create_directories("/Db/result/" + job_uuid);
    for (size_t piece_id = 0; piece_id < data.size(); ++piece_id)
    {
        auto ofs = std::ofstream("/Db/result/" + job_uuid + "/" + std::to_string(piece_id));
        ofs << data[piece_id];
        ofs.close();
    }

    auto cc_to_db = qmpc::ComputationToDb::Client::getInstance();
    auto read_data = cc_to_db->readModelparam(job_uuid);

    std::vector<std::string> true_result = {"1", "2", "3", "4"};
    EXPECT_EQ(true_result, read_data);

    initialize(job_uuid);
}

// model parameter(json)の取り出し
// nlohmann::json Client::readModelparamJson(const std::string &job_uuid);
TEST(ComputationToDbTest, SuccessReadModelParamJsonTest)
{
    const std::string job_uuid = "ReadModelParamTestId";
    initialize(job_uuid);

    const std::string data = R"({"result":"{\"key1\":1,\"key2\":{\"key3\":\"val\"}}")"
                             R"(,"meta":{"piece_id":0}})";
    fs::create_directories("/Db/result/" + job_uuid);
    auto ofs = std::ofstream("/Db/result/" + job_uuid + "/0");
    ofs << data;
    ofs.close();

    auto cc_to_db = qmpc::ComputationToDb::Client::getInstance();
    auto read_data = cc_to_db->readModelparamJson(job_uuid);

    nlohmann::json true_result = {{"key1", 1}, {"key2", {{"key3", "val"}}}};
    EXPECT_EQ(true_result, read_data);

    initialize(job_uuid);
}
TEST(ComputationToDbTest, SuccessReadModelParamJsonPieceTest)
{
    const std::string job_uuid = "ReadModelParamTestId";
    initialize(job_uuid);

    const std::vector<std::string> data = {
        R"({"result":"{\"key1\"")"
        R"(,"meta":{"piece_id":0}})",
        R"({"result":":1,\"key2\":{\"ke")"
        R"(,"meta":{"piece_id":1}})",
        R"({"result":"y3\":\"val\"}}")"
        R"(,"meta":{"piece_id":2}})",
    };
    fs::create_directories("/Db/result/" + job_uuid);
    for (size_t piece_id = 0; piece_id < data.size(); ++piece_id)
    {
        auto ofs = std::ofstream("/Db/result/" + job_uuid + "/" + std::to_string(piece_id));
        ofs << data[piece_id];
        ofs.close();
    }

    auto cc_to_db = qmpc::ComputationToDb::Client::getInstance();
    auto read_data = cc_to_db->readModelparamJson(job_uuid);

    nlohmann::json true_result = {{"key1", 1}, {"key2", {{"key3", "val"}}}};
    EXPECT_EQ(true_result, read_data);

    initialize(job_uuid);
}

// Job を DB に新規登録する
// void Client::registerJob(const std::string &job_uuid, const int &status);
TEST(ComputationToDbTest, SuccessRregisterJobTest)
{
    const std::string job_uuid = "RregisterJobTestId";
    initialize(job_uuid);

    auto cc_to_db = qmpc::ComputationToDb::Client::getInstance();
    cc_to_db->registerJob(job_uuid, pb_common_types::JobStatus::UNKNOWN);

    auto exist = fs::exists("/Db/result/" + job_uuid);
    EXPECT_TRUE(exist);

    initialize(job_uuid);
}

// Job の実行状態を更新する
// void Client::updateJobStatus(const std::string &job_uuid, const int &status);
TEST(ComputationToDbTest, SuccessupdateJobStatusTest)
{
    const std::string job_uuid = "RregisterJobTestId";
    initialize(job_uuid);

    fs::create_directories("/Db/result/" + job_uuid);
    const google::protobuf::EnumDescriptor *descriptor =
        google::protobuf::GetEnumDescriptor<pb_common_types::JobStatus>();
    for (int status = 0;; ++status)
    {
        auto ptr = descriptor->FindValueByNumber(status);
        if (ptr == nullptr) break;

        auto cc_to_db = qmpc::ComputationToDb::Client::getInstance();
        cc_to_db->updateJobStatus(job_uuid, status);

        auto exist = fs::exists("/Db/result/" + job_uuid + "/status_" + ptr->name());
        EXPECT_TRUE(exist);
    }

    initialize(job_uuid);
}

// resultの保存
// template <class T>
// void writeComputationResult(const std::string &job_uuid, const T &results, int piece_size);
TEST(ComputationToDbTest, SuccessWriteComputationResultArrayTest)
{
    const std::string job_uuid = "WriteComputationResultTestId";
    initialize(job_uuid);

    const std::vector<std::vector<std::string>> data = {{"12", "15", "21"}};
    fs::create_directories("/Db/result/" + job_uuid);

    auto cc_to_db = qmpc::ComputationToDb::Client::getInstance();
    cc_to_db->writeComputationResult(job_uuid, data);

    auto ifs = std::ifstream("/Db/result/" + job_uuid + "/0");
    std::string read_data;
    getline(ifs, read_data);
    const auto true_data = R"({"job_uuid":"WriteComputationResultTestId")"
                           R"(,"meta":{"piece_id":0})"
                           R"(,"result":"[[\"12\",\"15\",\"21\"]]"})";
    EXPECT_EQ(read_data, true_data);

    initialize(job_uuid);
}

TEST(ComputationToDbTest, SuccessWriteComputationResultJsonTest)
{
    const std::string job_uuid = "WriteComputationResultTestId";
    initialize(job_uuid);

    nlohmann::json data = {{"key1", 1}, {"key2", {{"key3", "val"}}}};
    fs::create_directories("/Db/result/" + job_uuid);

    auto cc_to_db = qmpc::ComputationToDb::Client::getInstance();
    cc_to_db->writeComputationResult(job_uuid, data);

    auto ifs = std::ifstream("/Db/result/" + job_uuid + "/0");
    std::string read_data;
    getline(ifs, read_data);
    const auto true_data = R"({"job_uuid":"WriteComputationResultTestId")"
                           R"(,"meta":{"piece_id":0})"
                           R"(,"result":"{\"key1\":1,\"key2\":{\"key3\":\"val\"}}"})";
    EXPECT_EQ(read_data, true_data);

    initialize(job_uuid);
}

TEST(ComputationToDbTest, SuccessWriteComputationResultArrayPieceTest)
{
    const std::string job_uuid = "WriteComputationResultTestId";
    initialize(job_uuid);

    const std::vector<std::vector<std::string>> data = {{"12", "15", "21"}};
    fs::create_directories("/Db/result/" + job_uuid);

    auto cc_to_db = qmpc::ComputationToDb::Client::getInstance();
    cc_to_db->writeComputationResult(job_uuid, data, 4);

    std::vector<std::string> true_result = {R"([[\"1)", R"(2\",\")", R"(15\",)", R"(\"21\")", "]]"};
    for (size_t piece_id = 0; piece_id < true_result.size(); ++piece_id)
    {
        auto ifs = std::ifstream("/Db/result/" + job_uuid + "/" + std::to_string(piece_id));
        std::string read_data;
        getline(ifs, read_data);
        const auto true_data = R"({"job_uuid":"WriteComputationResultTestId")"
                               R"(,"meta":{"piece_id":)"
                               + std::to_string(piece_id) + R"(},"result":")"
                               + true_result[piece_id] + R"("})";
        EXPECT_EQ(read_data, true_data);
    }

    initialize(job_uuid);
}

TEST(ComputationToDbTest, SuccessWriteComputationResultJsonPieceTest)
{
    const std::string job_uuid = "WriteComputationResultTestId";
    initialize(job_uuid);

    nlohmann::json data = {{"key1", 1}, {"key2", {{"key3", "val"}}}};
    fs::create_directories("/Db/result/" + job_uuid);

    auto cc_to_db = qmpc::ComputationToDb::Client::getInstance();
    cc_to_db->writeComputationResult(job_uuid, data, 4);

    std::vector<std::string> true_result = {
        R"({\"ke)",
        R"(y1\":)",
        R"(1,\"k)",
        R"(ey2\")",
        R"(:{\"k)",
        R"(ey3\")",
        R"(:\"va)",
        R"(l\"}})"};
    for (size_t piece_id = 0; piece_id < true_result.size(); ++piece_id)
    {
        auto ifs = std::ifstream("/Db/result/" + job_uuid + "/" + std::to_string(piece_id));
        std::string read_data;
        getline(ifs, read_data);
        const auto true_data = R"({"job_uuid":"WriteComputationResultTestId")"
                               R"(,"meta":{"piece_id":)"
                               + std::to_string(piece_id) + R"(},"result":")"
                               + true_result[piece_id] + R"("})";
        EXPECT_EQ(read_data, true_data);
    }

    initialize(job_uuid);
}

TEST(ComputationToDbTest, SuccessWriteComputationResultCompletedTest)
{
    const std::string job_uuid = "WriteComputationResultTestId";
    initialize(job_uuid);

    const std::vector<std::vector<std::string>> data = {{"12", "15", "21"}};
    fs::create_directories("/Db/result/" + job_uuid);

    auto cc_to_db = qmpc::ComputationToDb::Client::getInstance();
    cc_to_db->writeComputationResult(job_uuid, data);

    auto exist = fs::exists("/Db/result/" + job_uuid + "/completed");
    EXPECT_TRUE(exist);

    initialize(job_uuid);
}
