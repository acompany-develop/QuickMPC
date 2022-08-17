#include "Client/AnyToDb/Client.hpp"
#include "Client/AnyToDb/N1QL.hpp"
#include "Client/ComputationToDbGate/Client.hpp"
#include "gtest/gtest.h"
#include "nlohmann/json.hpp"

TEST(ComputationToDbGateTest, PieceIdTest)
{
    // dataを3回分けて送信
    auto any_to_db = AnyToDb::Client("sharedb");
    auto n1ql = AnyToDb::N1QL("share");
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
    auto any_to_db = AnyToDb::Client("sharedb");
    auto n1ql = AnyToDb::N1QL("share");
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
    // any_to_db.executeQuery(n1ql.delete_order(1000000));
}

TEST(ComputationToDbGateTest, ReadModelParamJsoneTest)
{
    // setting
    auto any_to_db = AnyToDb::Client("sharedb");
    auto n1ql = AnyToDb::N1QL("result");
    auto job_uuid = "read_json_test_id";
    any_to_db.executeQuery(n1ql.delete_id("job_uuid", job_uuid));

    // read用データ送信
    nlohmann::json data{{"val1", "2"}, {"to", {{"val2", "5"}, {"val3", "7"}}}};
    nlohmann::json data_json{{"job_uuid", job_uuid}, {"result", data}};
    std::string data_str = data_json.dump();
    any_to_db.executeQuery(n1ql.insert(data_str));

    // readして比較
    auto cc_to_db = qmpc::ComputationToDbGate::Client::getInstance();
    auto read_data = cc_to_db->readModelparamJson(job_uuid);
    EXPECT_EQ(read_data, data);

    any_to_db.executeQuery(n1ql.delete_id("job_uuid", job_uuid));
}
