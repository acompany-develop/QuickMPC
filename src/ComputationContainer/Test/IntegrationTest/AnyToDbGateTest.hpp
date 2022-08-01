#pragma once

#include "gtest/gtest.h"

#include <vector>
#include <string>

#include "nlohmann/json.hpp"

#include "Client/AnyToDb/Client.hpp"
#include "Client/AnyToDb/N1QL.hpp"

#include "LogHeader/Logger.hpp"

// N1QLのによるクエリ文字列
const auto insert_query_string = std::string(
    "INSERT INTO `share` (KEY, VALUE) "
    "VALUES (UUID(), { "
    "\"test_id\": \"TestExecuteQuery\", "
    "\"a\" : 1, \"b\": 2, \"c\": 3 }) "
    "RETURNING *;"
);

const auto select_query_string = std::string(
    "SELECT x.*, meta().id "
    "FROM `share` x "
    "WHERE x.test_id = \"TestExecuteQuery\";"
);

const auto delete_query_string = std::string(
    "DELETE FROM `share` x "
    "WHERE x.test_id = \"TestExecuteQuery\" "
    "RETURNING x.*, meta().id ;"
);

// 以下生文字列クエリによるTest
TEST(AnyToDbGateTest, ExecuteQueryTest)
{
    const auto client = AnyToDb::Client("sharedb");
    EXPECT_NO_THROW(client.executeQuery(insert_query_string));
    EXPECT_NO_THROW(auto result_select = client.executeQuery(select_query_string));
    EXPECT_NO_THROW(auto result_delete = client.executeQuery(delete_query_string));
}

// 以下N1QLクラスで生成したクエリによるTest
TEST(AnyToDbGateTest, N1QLQueryTest)
{
    // dataをjsonに変換
    std::vector<std::string> data = {"12", "15", "21"};
    nlohmann::json data_json;
    data_json["job_uuid"] = "hoge";
    data_json["meta"];
    data_json["result"] = data;
    AnyToDb::N1QLValue n1ql_data(data_json.dump());

    // DbGateと通信
    const auto client = AnyToDb::Client("sharedb");
    const auto n1ql = AnyToDb::N1QL("share");
    EXPECT_NO_THROW(client.executeQuery(n1ql.insert(data_json.dump())));
    EXPECT_NO_THROW(client.executeQuery(n1ql.insert(n1ql_data)));
    EXPECT_NO_THROW(client.executeQuery(n1ql.select()));
    EXPECT_NO_THROW(client.executeQuery(n1ql.select_id("job_uuid", "hoge")));
    EXPECT_NO_THROW(client.executeQuery(n1ql.delete_id("job_uuid", "hoge")));
}

// Bulkインサートのテスト
TEST(AnyToDbGateTest, BulkInsertTest)
{
    const auto client = AnyToDb::Client("sharedb");
    const auto n1ql = AnyToDb::N1QL("share");

    // とりあえず全部消す
    EXPECT_NO_THROW(client.executeQuery("DELETE FROM share WHERE 1;"));

    std::vector<AnyToDb::N1QLValue> datas_vec;
    std::vector<std::string> datas_true_vec;
    const int N = 3;

    for (int i = 0; i < N; i++)
    {
        nlohmann::json data_json;
        data_json["a"] = 123 + i * 1000;
        data_json["b"] = 456 + i * 1000;
        data_json["c"] = 789 + i * 1000;
        AnyToDb::N1QLValue n1ql_data(data_json.dump(), std::to_string(i * 111));
        datas_vec.push_back(n1ql_data);
        datas_true_vec.push_back(data_json.dump());
    }

    EXPECT_NO_THROW(client.executeQuery(n1ql.bulkinsert(datas_vec)));

    // 確認
    auto datas_result = nlohmann::json::parse(client.executeQuery(n1ql.delete_order(N)));
    for (int i = 0; i < N; i++)
    {
        auto data_val_true = nlohmann::json::parse(datas_true_vec[i]);
        ASSERT_EQ(data_val_true["a"], datas_result[i]["a"]);
        ASSERT_EQ(data_val_true["b"], datas_result[i]["b"]);
        ASSERT_EQ(data_val_true["c"], datas_result[i]["c"]);
    }
}

// 例外Test
TEST(AnyToDbGateTest, ThrowTest)
{
    AnyToDb::N1QLValue error_data("e");
    const auto error_client = AnyToDb::Client("E");
    const auto client = AnyToDb::Client("sharedb");
    const auto n1ql = AnyToDb::N1QL("share");

    // DbGateと通信
    EXPECT_ANY_THROW(client.executeQuery(n1ql.insert(error_data)));
    EXPECT_ANY_THROW(error_client.executeQuery(n1ql.select_id("job_uuid", "hoge")));
    EXPECT_ANY_THROW(error_client.executeQuery(n1ql.delete_id("job_uuid", "hoge")));
}

// InjectionTest
TEST(AnyToDbGateTest, InjectionTest)
{
    const auto client_share = AnyToDb::Client("sharedb");
    const std::string job_uuid = "123456";
    nlohmann::json result_json;
    result_json["a"] = "412863086";
    result_json["b"] = "209173902";
    result_json["c"] = "798369118";
    std::string result_str = result_json.dump();
    std::vector<std::string> results;
    results.emplace_back(result_str);

    nlohmann::json data_json;
    data_json["job_uuid"] = job_uuid;
    data_json["result"] = results;
    data_json["test_id"] = "AnyToDbGateTest:InjectionTest";

    const auto n1ql = AnyToDb::N1QL("result");
    // Sampleデータを挿入しておく
    const auto insert_query_string = n1ql.insert(data_json.dump());
    client_share.executeQuery(insert_query_string);
    client_share.executeQuery(insert_query_string);
    client_share.executeQuery(insert_query_string);

    // DbGateと通信
    // WHERE句のインジェクション
    try
    {
        auto result_select = client_share.executeQuery(n1ql.select_id("job_uuid", "' OR ''='"));
        ASSERT_EQ("[]", result_select);  // 対策成功
    }
    catch (const std::exception &e)
    {
        // 対策成功
        spdlog::info("caught {}", e.what());
    }

    try
    {
        auto result_select = client_share.executeQuery(n1ql.select_id("job_uuid", "' OR \"\"='"));
        ASSERT_EQ("[]", result_select);  // 対策成功
    }
    catch (const std::exception &e)
    {
        // 対策成功
        spdlog::info("caught {}", e.what());
    }

    try
    {
        auto result_select = client_share.executeQuery(n1ql.select_id("job_uuid", "\" OR ''=\""));
        ASSERT_EQ("[]", result_select);  // 対策成功
    }
    catch (const std::exception &e)
    {
        // 対策成功
        spdlog::info("caught {}", e.what());
    }

    try
    {
        auto result_select = client_share.executeQuery(n1ql.select_id("job_uuid", "\" OR \"\"=\""));
        ASSERT_EQ("[]", result_select);  // 対策成功
    }
    catch (const std::exception &e)
    {
        // 対策成功
        spdlog::info("caught {}", e.what());
    }

    const auto delete_query_string = n1ql.delete_id("test_id", "AnyToDbGateTest:InjectionTest");
    client_share.executeQuery(delete_query_string);

    // SELECT句のインジェクション
    // 今の設計ではSELECT句に埋め込むインタフェースが存在しないため不要
}

// selectのテスト
TEST(AnyToDbGateTest, SelectTest)
{
    std::vector<std::string> data_1 = {"12", "15", "21"};
    std::vector<std::string> data_2 = {"13", "14", "25"};
    nlohmann::json data_json_1;
    nlohmann::json data_json_2;
    data_json_1["job_uuid"] = "hoge";
    data_json_1["meta"];
    data_json_1["result"] = data_1;
    data_json_2["job_uuid"] = "fuga";
    data_json_2["meta"];
    data_json_2["result"] = data_2;

    // DbGateと通信
    const auto client = AnyToDb::Client("sharedb");
    const auto n1ql = AnyToDb::N1QL("share");
    client.executeQuery("DELETE FROM `share`");

    client.executeQuery(n1ql.insert(data_json_1.dump()));
    auto result1 = client.executeQuery(n1ql.select());
    auto res_json1 = nlohmann::json::parse(result1);
    std::string cnt1 = res_json1[0]["job_uuid"].get<std::string>();
    EXPECT_EQ("hoge", cnt1);

    client.executeQuery(n1ql.insert(data_json_2.dump()));
    auto result2 = client.executeQuery(n1ql.select());
    auto res_json2 = nlohmann::json::parse(result2);
    cnt1 = res_json2[0]["job_uuid"].get<std::string>();
    std::string cnt2 = res_json2[1]["job_uuid"].get<std::string>();
    EXPECT_EQ("hoge", cnt1);
    EXPECT_EQ("fuga", cnt2);

    client.executeQuery(n1ql.delete_id("job_uuid", "hoge"));
    client.executeQuery(n1ql.delete_id("job_uuid", "fuga"));
    auto result3 = client.executeQuery(n1ql.select());
    auto res_json3 = nlohmann::json::parse(result3);
    int cnt3 = res_json3.size();
    EXPECT_EQ(0, cnt3);
}

// 要素数を数える
TEST(AnyToDbGateTest, CountTest)
{
    /*
    SELECT COUNT(*) FROM `bucket`
    上記のクエリは叩かない 詳細はREADMEの注意
    */

    // dataをjsonに変換
    std::vector<std::string> data = {"12", "15", "21"};
    nlohmann::json data_json;
    data_json["job_uuid"] = "hoge";
    data_json["meta"];
    data_json["result"] = data;

    // DbGateと通信
    const auto client = AnyToDb::Client("sharedb");
    const auto n1ql = AnyToDb::N1QL("share");
    client.executeQuery(n1ql.delete_id("job_uuid", "hoge"));

    // insert -> count==1 -> insert -> count==2 -> delete -> count==0
    client.executeQuery(n1ql.insert(data_json.dump()));
    auto result1 = client.executeQuery(n1ql.select());
    auto res_json1 = nlohmann::json::parse(result1);
    int cnt1 = res_json1.size();
    EXPECT_EQ(1, cnt1);

    client.executeQuery(n1ql.insert(data_json.dump()));
    auto result2 = client.executeQuery(n1ql.select());
    auto res_json2 = nlohmann::json::parse(result2);
    int cnt2 = res_json2.size();
    EXPECT_EQ(2, cnt2);

    client.executeQuery(n1ql.delete_id("job_uuid", "hoge"));
    auto result3 = client.executeQuery(n1ql.select());
    auto res_json3 = nlohmann::json::parse(result3);
    int cnt3 = res_json3.size();
    EXPECT_EQ(0, cnt3);
}

TEST(AnyToDbGateTest, DeleteOrderTest)
{
    // DbGateと通信
    const auto client = AnyToDb::Client("sharedb");
    const auto n1ql = AnyToDb::N1QL("share");
    nlohmann::json data_json;
    std::string data_str;

    // 挿入順が 4-3-2-1
    data_json["test_id"] = 4;
    client.executeQuery(n1ql.insert(data_json.dump()));

    data_json["test_id"] = 3;
    client.executeQuery(n1ql.insert(data_json.dump()));

    data_json["test_id"] = 2;
    client.executeQuery(n1ql.insert(data_json.dump()));

    data_json["test_id"] = 1;
    client.executeQuery(n1ql.insert(data_json.dump()));

    // 削除順も 4-3-2-1
    auto delete_result1 = client.executeQuery(n1ql.delete_order(1));
    auto res_json1 = nlohmann::json::parse(delete_result1);
    int test_id1 = res_json1[0]["test_id"].get<int>();
    EXPECT_EQ(4, test_id1);
    auto delete_result2to3 = client.executeQuery(n1ql.delete_order(2));
    auto res_json2to3 = nlohmann::json::parse(delete_result2to3);
    int test_id2 = res_json2to3[0]["test_id"].get<int>();
    int test_id3 = res_json2to3[1]["test_id"].get<int>();
    EXPECT_EQ(3, test_id2);
    EXPECT_EQ(2, test_id3);
    auto delete_result4 = client.executeQuery(n1ql.delete_order(1));
    auto res_json4 = nlohmann::json::parse(delete_result4);
    int test_id4 = res_json4[0]["test_id"].get<int>();
    EXPECT_EQ(1, test_id4);
}
