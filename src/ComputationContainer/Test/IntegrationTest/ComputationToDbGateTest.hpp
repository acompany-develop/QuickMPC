#include "gtest/gtest.h"
#include <map>
#include "nlohmann/json.hpp"
#include "Client/ComputationToDbGate/Client.hpp"
#include "Client/AnyToDb/Client.hpp"
#include "Client/AnyToDb/N1QL.hpp"

TEST(ComputationToDbGateTest, PieceIdTest)
{
    // 全て削除
    auto any_to_db = AnyToDb::Client("sharedb");
    auto n1ql = AnyToDb::N1QL("share");
    any_to_db.executeQuery(n1ql.delete_order(1000000));

    // dataを3回分けて送信
    std::vector<std::vector<std::vector<std::string>>> data = {
        {{"1", "2"}, {"3", "4"}}, {{"5", "6"}, {"7", "8"}}, {{"9", "10"}}};
    nlohmann::json data_json;
    auto data_id = "id";
    data_json["data_id"] = data_id;
    for (int i = 0; i < 3; ++i)
    {
        data_json["meta"]["piece_id"] = i;
        data_json["meta"]["schema"] = {"attr1", "attr2"};
        data_json["value"] = data[i];
        std::string data_str = data_json.dump();
        any_to_db.executeQuery(n1ql.insert(data_str));
    }

    // piece data read Test
    auto cc_to_db = qmpc::ComputationToDbGate::Client::getInstance();
    auto read_data = cc_to_db->readShare(data_id).getTable();
    std::vector<std::vector<std::string>> true_data = {
        {"1", "2"}, {"3", "4"}, {"5", "6"}, {"7", "8"}, {"9", "10"}};

    EXPECT_EQ(true_data, read_data);
    // 全て削除
    any_to_db.executeQuery(n1ql.delete_order(1000000));
}

TEST(ComputationToDbGateTest, StreamTest)
{
    // 全て削除
    auto any_to_db = AnyToDb::Client("sharedb");
    auto n1ql = AnyToDb::N1QL("share");
    any_to_db.executeQuery(n1ql.delete_order(1000000));

    std::vector<std::vector<std::vector<std::string>>> data;
    data.reserve(500);
    for (int i = 0; i < 500; i++)
    {
        std::vector<std::string> add_data;
        for (int j = 0; j < 500; j++)
        {
            add_data.push_back(std::to_string(j + 1));
        }
        std::vector<std::vector<std::string>> add_vec = {add_data};

        data.push_back(add_vec);
    }
    nlohmann::json data_json;
    auto data_id = "id";
    data_json["data_id"] = data_id;
    std::vector<std::string> schema;
    for (int i = 0; i < 500; i++)
    {
        schema.push_back("attr" + std::to_string(i + 1));
    }

    for (int i = 0; i < 500; ++i)
    {
        data_json["meta"]["piece_id"] = i;
        data_json["meta"]["schema"] = schema;
        data_json["value"] = data[i];
        std::string data_str = data_json.dump();
        std::string query = n1ql.insert(data_str);
        any_to_db.executeQuery(query);
    }

    // piece data read Test
    auto cc_to_db = qmpc::ComputationToDbGate::Client::getInstance();
    auto read_data = cc_to_db->readShare(data_id).getTable();
    std::vector<std::vector<std::string>> true_data;
    for (int i = 0; i < 500; i++)
    {
        std::vector<std::string> add_data;
        for (int j = 0; j < 500; j++)
        {
            add_data.push_back(std::to_string(j + 1));
        }
        true_data.push_back(add_data);
    }

    EXPECT_EQ(true_data, read_data);
    // 全て削除
    any_to_db.executeQuery(n1ql.delete_order(1000000));
}

TEST(ComputationToDbGateTest, ReadModelParamTest)
{
    // setting
    auto any_to_db = AnyToDb::Client("sharedb");
    auto n1ql = AnyToDb::N1QL("result");
    auto job_uuid = "read_test_id";
    any_to_db.executeQuery(n1ql.delete_id("job_uuid", job_uuid));

    // read用データ送信
    nlohmann::json data = std::vector<std::string>{"1", "2", "3"};
    auto data_dump = data.dump();
    nlohmann::json data_json{
        {"job_uuid", job_uuid}, {"result", data_dump}, {"meta", {"piece_id", 1}}};
    std::string data_str = data_json.dump();
    any_to_db.executeQuery(n1ql.insert(data_str));

    // readして比較
    auto cc_to_db = qmpc::ComputationToDbGate::Client::getInstance();
    auto read_data = cc_to_db->readModelparamJson(job_uuid);
    EXPECT_EQ(read_data, data_dump);

    any_to_db.executeQuery(n1ql.delete_id("job_uuid", job_uuid));
}

TEST(ComputationToDbGateTest, ReadModelParamPieceTest)
{
    // setting
    auto any_to_db = AnyToDb::Client("sharedb");
    auto n1ql = AnyToDb::N1QL("result");
    auto job_uuid = "read_test_id";
    any_to_db.executeQuery(n1ql.delete_id("job_uuid", job_uuid));

    // 2つに分割してread用データ送信
    nlohmann::json data = std::vector<std::string>{"1", "2", "3"};
    auto data_dump = data.dump();
    auto s1 = data_dump.substr(0, 5);
    auto s2 = data_dump.substr(5);

    nlohmann::json data_json1{{"job_uuid", job_uuid}, {"result", s1}, {"meta", {"piece_id", 1}}};
    std::string data_str1 = data_json1.dump();
    any_to_db.executeQuery(n1ql.insert(data_str1));

    nlohmann::json data_json2{{"job_uuid", job_uuid}, {"result", s2}, {"meta", {"piece_id", 2}}};
    std::string data_str2 = data_json2.dump();
    any_to_db.executeQuery(n1ql.insert(data_str2));

    // readして比較
    auto cc_to_db = qmpc::ComputationToDbGate::Client::getInstance();
    auto read_data = cc_to_db->readModelparamJson(job_uuid);
    EXPECT_EQ(read_data, data_dump);

    any_to_db.executeQuery(n1ql.delete_id("job_uuid", job_uuid));
}

TEST(ComputationToDbGateTest, ReadModelParamJsonTest)
{
    // setting
    auto any_to_db = AnyToDb::Client("sharedb");
    auto n1ql = AnyToDb::N1QL("result");
    auto job_uuid = "read_json_test_id";
    any_to_db.executeQuery(n1ql.delete_id("job_uuid", job_uuid));

    // read用データ送信
    nlohmann::json data{{"val1", "2"}, {"to", {{"val2", "5"}, {"val3", "7"}}}};
    auto data_dump = data.dump();
    nlohmann::json data_json{
        {"job_uuid", job_uuid}, {"result", data_dump}, {"meta", {"piece_id", 1}}};
    std::string data_str = data_json.dump();
    any_to_db.executeQuery(n1ql.insert(data_str));

    // readして比較
    auto cc_to_db = qmpc::ComputationToDbGate::Client::getInstance();
    auto read_data = cc_to_db->readModelparamJson(job_uuid);
    EXPECT_EQ(read_data, data_dump);

    any_to_db.executeQuery(n1ql.delete_id("job_uuid", job_uuid));
}

TEST(ComputationToDbGateTest, ReadModelParamJsonPieceTest)
{
    // setting
    auto any_to_db = AnyToDb::Client("sharedb");
    auto n1ql = AnyToDb::N1QL("result");
    auto job_uuid = "read_json_test_id";
    any_to_db.executeQuery(n1ql.delete_id("job_uuid", job_uuid));

    // 2つに分割してread用データ送信
    nlohmann::json data{{"val1", "2"}, {"to", {{"val2", "5"}, {"val3", "7"}}}};
    auto data_dump = data.dump();
    auto s1 = data_dump.substr(0, 10);
    auto s2 = data_dump.substr(5);

    nlohmann::json data_json1{{"job_uuid", job_uuid}, {"result", s1}, {"meta", {"piece_id", 1}}};
    std::string data_str1 = data_json1.dump();
    any_to_db.executeQuery(n1ql.insert(data_str1));

    nlohmann::json data_json2{{"job_uuid", job_uuid}, {"result", s2}, {"meta", {"piece_id", 2}}};
    std::string data_str2 = data_json2.dump();
    any_to_db.executeQuery(n1ql.insert(data_str2));

    // readして比較
    auto cc_to_db = qmpc::ComputationToDbGate::Client::getInstance();
    auto read_data = cc_to_db->readModelparamJson(job_uuid);
    EXPECT_EQ(read_data, data_dump);

    any_to_db.executeQuery(n1ql.delete_id("job_uuid", job_uuid));
}

TEST(ComputationToDbGateTest, RregisterJobTest)
{
    // setting
    auto any_to_db = AnyToDb::Client("sharedb");
    const auto n1ql = AnyToDb::N1QL("result");
    const std::string job_uuid = "register_job_test_id";
    int status = 1;
    any_to_db.executeQuery(n1ql.delete_id("job_uuid", job_uuid));

    // データ送信
    auto cc_to_db = qmpc::ComputationToDbGate::Client::getInstance();
    cc_to_db->registerJob(job_uuid, status);

    // readして比較
    auto res = any_to_db.executeQuery(n1ql.delete_id("job_uuid", job_uuid));
    auto res_json = nlohmann::json::parse(res)[0];
    EXPECT_EQ(res_json["status"], status);
    EXPECT_EQ(res_json["job_uuid"], job_uuid);
}

TEST(ComputationToDbGateTest, WriteComputationResultTest)
{
    // setting
    auto any_to_db = AnyToDb::Client("sharedb");
    const auto n1ql = AnyToDb::N1QL("result");
    const std::vector<std::vector<std::string>> data = {{"12", "15", "21"}};
    const std::string job_uuid = "write_computation_result_id";
    any_to_db.executeQuery(n1ql.delete_id("job_uuid", job_uuid));

    // データ送信
    auto cc_to_db = qmpc::ComputationToDbGate::Client::getInstance();
    cc_to_db->registerJob(job_uuid, 1);
    cc_to_db->writeComputationResult(job_uuid, data, 1000000);

    // readして比較
    auto res = any_to_db.executeQuery(n1ql.delete_id("job_uuid", job_uuid));
    std::string read_data = nlohmann::json::parse(res)[0]["result"];
    const auto data_str = "[[\"12\",\"15\",\"21\"]]";
    EXPECT_EQ(read_data, data_str);
}

TEST(ComputationToDbGateTest, PieceWriteComputationResultTest)
{
    // setting
    auto any_to_db = AnyToDb::Client("sharedb");
    const auto n1ql = AnyToDb::N1QL("result");
    const std::vector<std::vector<std::string>> data = {{"12", "15", "21"}};
    const std::string job_uuid = "piece_write_computation_result_id";
    any_to_db.executeQuery(n1ql.delete_id("job_uuid", job_uuid));

    // 5byteずつデータ送信
    auto cc_to_db = qmpc::ComputationToDbGate::Client::getInstance();
    cc_to_db->registerJob(job_uuid, 1);
    cc_to_db->writeComputationResult(job_uuid, data, 5);

    // readしてpiece_id順に結合して比較
    auto res = any_to_db.executeQuery(n1ql.delete_id("job_uuid", job_uuid));
    std::string read_data = "";
    std::map<int, std::string> mp;
    for (const auto r : nlohmann::json::parse(res))
    {
        mp.emplace(r["meta"]["piece_id"], r["result"]);
    }
    for (const auto& [_, s] : mp)
    {
        static_cast<void>(_);  // NOTE: unused warningを消すため
        read_data += s;
    }
    const auto data_str = "[[\"12\",\"15\",\"21\"]]";
    EXPECT_EQ(read_data, data_str);
}
