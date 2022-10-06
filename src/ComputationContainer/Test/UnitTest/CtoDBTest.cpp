#include <experimental/filesystem>
#include <fstream>

#include "Client/ComputationToDb/Client.hpp"
#include "gtest/gtest.h"
namespace fs = std::experimental::filesystem;

// XXX: 実装終わったら削除する
#include <iostream>
using std::cout;
using std::endl;

// DBを初期化する
auto initialize()
{
    for (const auto& entry : fs::directory_iterator("/Db/share/"))
    {
        if (fs::is_directory(entry.path())) fs::remove_all(entry.path());
    }
    for (const auto& entry : fs::directory_iterator("/Db/result/"))
    {
        if (fs::is_directory(entry.path())) fs::remove_all(entry.path());
    }
}

// shareの取り出し
// qmpc::ComputationToDbGate::ValueTable Client::readShare(const std::string &data_id);
TEST(ComputationToDbTest, SuccessReadShareTest)
{
    initialize();

    const std::string data_id = "ReadShareTestId";
    const std::string data =
        "{\"value\":[[\"1\",\"2\"],[\"3\",\"4\"]]"
        ",\"meta\":{\"piece_id\":0,\"schema\":[\"attr1\",\"attr2\"]}}";
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

    initialize();
}
TEST(ComputationToDbTest, SuccessReadSharePieceTest)
{
    initialize();

    const std::string data_id = "ReadShareTestId";
    const std::vector<std::string> data = {
        "{\"value\":[[\"1\",\"2\"],[\"3\",\"4\"]]"
        ",\"meta\":{\"piece_id\":0,\"schema\":[\"attr1\",\"attr2\"]}}",
        "{\"value\":[[\"5\",\"6\"],[\"7\",\"8\"]]"
        ",\"meta\":{\"piece_id\":1,\"schema\":[\"attr1\",\"attr2\"]}}",
        "{\"value\":[[\"9\",\"10\"]]"
        ",\"meta\":{\"piece_id\":2,\"schema\":[\"attr1\",\"attr2\"]}}",
    };
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

    initialize();
}
TEST(ComputationToDbTest, SuccessReadShareLargeTest)
{
    initialize();

    constexpr int H = 500;
    constexpr int W = 500;
    std::string data_id = "ReadShareTestId";
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

    initialize();
}

/*
// model parameter(vector)の取り出し
// std::vector<std::string> Client::readModelparam(const std::string &job_uuid);
TEST(ComputationToDbTest, SuccessReadModelParamTest) {}
TEST(ComputationToDbTest, SuccessReadModelParamPieceTest) {}

// model parameter(json)の取り出し
// nlohmann::json Client::readModelparamJson(const std::string &job_uuid);
TEST(ComputationToDbTest, SuccessReadModelParamJsonTest) {}
TEST(ComputationToDbTest, SuccessReadModelParamJsonPieceTest) {}

// Job を DB に新規登録する
// void Client::registerJob(const std::string &job_uuid, const int &status);
TEST(ComputationToDbTest, SuccessRregisterJobTest) {}

// Job の実行状態を更新する
// void Client::updateJobStatus(const std::string &job_uuid, const int &status);
TEST(ComputationToDbTest, SuccessupdateJobStatusTest) {}

// resultの保存
// template <class T>
// void writeComputationResult(const std::string &job_uuid, const T &results, int piece_size);
TEST(ComputationToDbTest, SuccessWriteComputationResultTest) {}
TEST(ComputationToDbTest, SuccessWriteComputationResultPieceTest) {}
*/
