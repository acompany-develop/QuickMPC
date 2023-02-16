#include <experimental/filesystem>
#include <string>
#include <vector>

#include "client/computation_to_db/value_table.hpp"
#include "gtest/gtest.h"
namespace fs = std::experimental::filesystem;

using Schema = std::vector<std::string>;
using Table = std::vector<std::vector<std::string>>;

/******************** 前処理，後処理 **********************/
class ValueTableTest : public testing::Test
{
protected:
    // Testに用いる{schema, table}のリスト
    const std::vector<std::pair<Schema, Table>> test_tables{
        {{"id", "attr1", "attr2"}, {{"101", "1", "2"}, {"102", "3", "4"}}},
        {{"id", "attr1", "attr3"}, {{"101", "5", "6"}, {"103", "7", "8"}}},
        {{"id", "attr1", "attr3", "attr4"}, {{"102", "9", "10", "11"}, {"103", "12", "13", "14"}}},
        {{"id", "attr3", "attr4", "attr5"},
         {{"103", "15", "16", "17"}, {"104", "18", "19", "20"}, {"105", "21", "22", "23"}}},
        {{"id", "attr1", "attr2"}, {{"101", "1", "0"}, {"102", "0", "4"}}},
        {{"id", "attr1", "attr2"}, {{"102", "0", "4"}, {"102", "1", "0"}}},
    };
    std::deque<std::string> data_ids;

    static auto initialize(const std::string& id)
    {
        fs::remove_all("/db/share/" + id);
        fs::remove_all("/db/result/" + id);
    }

    auto genValueTable(int table_itr)
    {
        // 初期化してリストに登録する(テスト後に削除するため)
        auto data_id = "ValueTableTest" + std::to_string(table_itr);
        initialize(data_id);
        data_ids.emplace_back(data_id);

        //テストで使用するデータをDBに保存する
        const int piece_id = 0;
        auto [schema, table] = test_tables[table_itr];
        nlohmann::json data_json = {
            {"value", table}, {"meta", {{"piece_id", piece_id}, {"schema", schema}}}};
        const std::string data = data_json.dump();

        auto data_path = "/db/share/" + data_id;
        fs::create_directories(data_path);
        std::ofstream ofs(data_path + "/" + std::to_string(piece_id));
        ofs << data;
        ofs.close();

        return qmpc::ComputationToDb::ValueTable(data_id);
    }

    void TearDown() override
    {
        // テストで使用したデータを削除する
        while (!data_ids.empty())
        {
            initialize(data_ids.front());
            data_ids.pop_front();
        }
    }
};

// TODO: Parameterテストを導入してテストデータを各テストごとに設定する
/********************** Test **********************/
TEST_F(ValueTableTest, vjoinTest)
{
    {
        auto vt1 = genValueTable(0);
        auto vt2 = genValueTable(1);
        auto join_table = vt1.vjoin(vt2, 1, 1);

        const std::vector<std::vector<std::string>> expect_table{
            {"101", "1"}, {"102", "3"}, {"103", "7"}};
        const std::vector<std::string> expect_schema{"id", "attr1"};
        EXPECT_EQ(join_table.getTable(), expect_table);
        EXPECT_EQ(join_table.getSchemas(), expect_schema);
        initialize(join_table.getDataId());
    }
    {
        auto vt1 = genValueTable(2);
        auto vt2 = genValueTable(3);
        auto join_table = vt1.vjoin(vt2, 1, 1);

        const std::vector<std::vector<std::string>> expect_table{
            {"102", "10", "11"}, {"103", "13", "14"}, {"104", "18", "19"}, {"105", "21", "22"}};
        const std::vector<std::string> expect_schema{"id", "attr3", "attr4"};
        EXPECT_EQ(join_table.getTable(), expect_table);
        EXPECT_EQ(join_table.getSchemas(), expect_schema);
        initialize(join_table.getDataId());
    }
}

TEST_F(ValueTableTest, hjoinTest)
{
    {
        auto vt1 = genValueTable(0);
        auto vt2 = genValueTable(1);
        auto join_table = vt1.hjoin(vt2, 1, 1);

        const std::vector<std::vector<std::string>> expect_table{{"101", "1", "2", "5", "6"}};
        const std::vector<std::string> expect_schema{"id", "attr1", "attr2", "attr1", "attr3"};
        EXPECT_EQ(join_table.getTable(), expect_table);
        EXPECT_EQ(join_table.getSchemas(), expect_schema);
        initialize(join_table.getDataId());
    }
    {
        auto vt1 = genValueTable(2);
        auto vt2 = genValueTable(3);
        auto join_table = vt1.hjoin(vt2, 1, 1);

        const std::vector<std::vector<std::string>> expect_table{
            {"103", "12", "13", "14", "15", "16", "17"}};
        const std::vector<std::string> expect_schema{
            "id", "attr1", "attr3", "attr4", "attr3", "attr4", "attr5"};
        EXPECT_EQ(join_table.getTable(), expect_table);
        EXPECT_EQ(join_table.getSchemas(), expect_schema);
        initialize(join_table.getDataId());
    }
}

TEST_F(ValueTableTest, vhjoinTest)
{
    auto vt1 = genValueTable(0);
    auto vt2 = genValueTable(1);
    auto vt3 = genValueTable(2);
    auto join_table = vt1.vjoin(vt2, 1, 1).hjoin(vt3, 1, 1);

    const std::vector<std::vector<std::string>> expect_table{
        {"102", "3", "9", "10", "11"}, {"103", "7", "12", "13", "14"}};
    const std::vector<std::string> expect_schema{"id", "attr1", "attr1", "attr3", "attr4"};
    EXPECT_EQ(join_table.getTable(), expect_table);
    EXPECT_EQ(join_table.getSchemas(), expect_schema);
    initialize(join_table.getDataId());
}

TEST_F(ValueTableTest, hvjoinTest)
{
    auto vt1 = genValueTable(0);
    auto vt2 = genValueTable(1);
    auto vt3 = genValueTable(2);
    auto join_table = vt1.hjoin(vt2, 1, 1).vjoin(vt3, 1, 1);

    const std::vector<std::vector<std::string>> expect_table{
        {"101", "1", "5", "6"}, {"102", "9", "9", "10"}, {"103", "12", "12", "13"}};
    const std::vector<std::string> expect_schema{"id", "attr1", "attr1", "attr3"};
    EXPECT_EQ(join_table.getTable(), expect_table);
    EXPECT_EQ(join_table.getSchemas(), expect_schema);
    initialize(join_table.getDataId());
}

TEST_F(ValueTableTest, vjoinColumnTest)
{
    auto vt1 = genValueTable(0);
    auto vt2 = genValueTable(4);
    {
        auto join_table = vt1.vjoin(vt2, 2, 2);

        const std::vector<std::vector<std::string>> expect_table{
            {"101", "1", "2"}, {"102", "3", "4"}, {"102", "0", "4"}};
        const std::vector<std::string> expect_schema{"id", "attr1", "attr2"};
        EXPECT_EQ(join_table.getTable(), expect_table);
        EXPECT_EQ(join_table.getSchemas(), expect_schema);
        initialize(join_table.getDataId());
    }
    {
        auto join_table = vt1.vjoin(vt2, 3, 3);

        const std::vector<std::vector<std::string>> expect_table{
            {"101", "1", "2"}, {"102", "3", "4"}, {"101", "1", "0"}};
        const std::vector<std::string> expect_schema{"id", "attr1", "attr2"};
        EXPECT_EQ(join_table.getTable(), expect_table);
        EXPECT_EQ(join_table.getSchemas(), expect_schema);
        initialize(join_table.getDataId());
    }
}

TEST_F(ValueTableTest, hjoinColumnTest)
{
    auto vt1 = genValueTable(0);
    auto vt2 = genValueTable(4);
    {
        auto join_table = vt1.hjoin(vt2, 2, 2);

        const std::vector<std::vector<std::string>> expect_table{{"101", "1", "2", "101", "0"}};
        const std::vector<std::string> expect_schema{"id", "attr1", "attr2", "id", "attr2"};
        EXPECT_EQ(join_table.getTable(), expect_table);
        EXPECT_EQ(join_table.getSchemas(), expect_schema);
        initialize(join_table.getDataId());
    }
    {
        auto join_table = vt1.hjoin(vt2, 3, 3);

        const std::vector<std::vector<std::string>> expect_table{{"102", "3", "4", "102", "0"}};
        const std::vector<std::string> expect_schema{"id", "attr1", "attr2", "id", "attr1"};
        EXPECT_EQ(join_table.getTable(), expect_table);
        EXPECT_EQ(join_table.getSchemas(), expect_schema);
        initialize(join_table.getDataId());
    }
}

TEST_F(ValueTableTest, hjoinShareTest)
{
    {
        auto vt1 = genValueTable(0);
        auto vt2 = genValueTable(1);
        auto join_table = vt1.hjoinShare(vt2, 1, 1);

        auto expect = vt1.hjoin(vt2, 1, 1);
        EXPECT_EQ(join_table.getTable(), expect.getTable());
        EXPECT_EQ(join_table.getSchemas(), expect.getSchemas());
        initialize(join_table.getDataId());
    }
    {
        auto vt1 = genValueTable(2);
        auto vt2 = genValueTable(3);
        auto join_table = vt1.hjoinShare(vt2, 1, 1);

        auto expect = vt1.hjoin(vt2, 1, 1);
        EXPECT_EQ(join_table.getTable(), expect.getTable());
        EXPECT_EQ(join_table.getSchemas(), expect.getSchemas());
        initialize(join_table.getDataId());
    }
}

TEST_F(ValueTableTest, hjoinShareColumnTest)
{
    {
        auto vt1 = genValueTable(0);
        auto vt2 = genValueTable(5);
        auto join_table = vt1.hjoinShare(vt2, 2, 2);

        auto expect = vt1.hjoin(vt2, 1, 1);
        EXPECT_EQ(join_table.getTable(), expect.getTable());
        EXPECT_EQ(join_table.getSchemas(), expect.getSchemas());
        initialize(join_table.getDataId());
    }
    {
        auto vt1 = genValueTable(0);
        auto vt2 = genValueTable(4);
        auto join_table = vt1.hjoinShare(vt2, 3, 3);

        auto expect = vt1.hjoin(vt2, 1, 1);
        EXPECT_EQ(join_table.getTable(), expect.getTable());
        EXPECT_EQ(join_table.getSchemas(), expect.getSchemas());
        initialize(join_table.getDataId());
    }
}

TEST_F(ValueTableTest, hjoinMultiple)
{
    auto vt1 = genValueTable(0);
    auto vt2 = genValueTable(1);
    auto join_table = vt1.hjoin(vt2, 1, 1);
    const std::vector<std::vector<std::string>> expect_table{{"101", "1", "2", "5", "6"}};
    const std::vector<std::string> expect_schema{"id", "attr1", "attr2", "attr1", "attr3"};

    // 同じ結合を何回も行う
    for (int i = 0; i < 10; ++i)
    {
        auto join_table = vt1.hjoin(vt2, 1, 1);
        EXPECT_EQ(join_table.getTable(), expect_table);
        EXPECT_EQ(join_table.getSchemas(), expect_schema);
    }
    initialize(join_table.getDataId());
}
