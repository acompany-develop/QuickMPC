#include <experimental/filesystem>
#include <fstream>

#include "gtest/gtest.h"
#include "job/table_data/value_table.hpp"

namespace fs = std::experimental::filesystem;

// DBを初期化する
auto initialize(const std::string& id)
{
    fs::remove_all("/db/share/" + id);
    fs::remove_all("/db/result/" + id);
}

#include <iostream>
using std::cout;
using std::endl;

TEST(ValueTableTest, SuccessBegin)
{
    const std::string data_id = "SuccessBegin";
    initialize(data_id);

    const std::string data = R"({"value":[["1","2"],["3","4"]])"
                             R"(,"meta":{"piece_id":0,"schema":["attr1","attr2"]}})";
    fs::create_directories("/db/share/" + data_id);
    auto ofs = std::ofstream("/db/share/" + data_id + "/0");
    ofs << data;
    ofs.close();

    auto table = qmpc::ComputationToDb::ValueTable(data_id);

    std::vector<std::string> true_row = {"1", "2"};
    EXPECT_EQ(*table.begin(), true_row);

    initialize(data_id);
}

TEST(ValueTableTest, SuccessIterate)
{
    const std::string data_id = "SuccessEnd";
    initialize(data_id);

    const std::string data = R"({"value":[["1","2"],["3","4"]])"
                             R"(,"meta":{"piece_id":0,"schema":["attr1","attr2"]}})";
    fs::create_directories("/db/share/" + data_id);
    auto ofs = std::ofstream("/db/share/" + data_id + "/0");
    ofs << data;
    ofs.close();

    auto table = qmpc::ComputationToDb::ValueTable(data_id);

    int i = 0;
    std::vector<std::vector<std::string>> true_table = {{"1", "2"}, {"3", "4"}};
    for (const auto& row : table)
    {
        EXPECT_EQ(row, true_table[i]);
        ++i;
    }
    // 全ての行分取得できたかどうか
    EXPECT_EQ(i, 2);

    initialize(data_id);
}

TEST(ValueTableTest, SuccessPieceIterate)
{
    const std::string data_id = "SuccessPieceIterate";
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

    auto table = qmpc::ComputationToDb::ValueTable(data_id);

    int i = 0;
    std::vector<std::vector<std::string>> true_table = {
        {"1", "2"}, {"3", "4"}, {"5", "6"}, {"7", "8"}, {"9", "10"}};
    for (const auto& row : table)
    {
        EXPECT_EQ(row, true_table[i]);
        ++i;
    }
    // 全ての行分取得できたかどうか
    EXPECT_EQ(i, 5);

    initialize(data_id);
}

TEST(ValueTableTest, SuccessGetTable)
{
    const std::string data_id = "SuccessGetTable";
    initialize(data_id);

    const std::string data = R"({"value":[["1","2"],["3","4"]])"
                             R"(,"meta":{"piece_id":0,"schema":["attr1","attr2"]}})";
    fs::create_directories("/db/share/" + data_id);
    auto ofs = std::ofstream("/db/share/" + data_id + "/0");
    ofs << data;
    ofs.close();

    auto table = qmpc::ComputationToDb::ValueTable(data_id);
    auto get_table = table.getTable();

    std::vector<std::vector<std::string>> true_table = {{"1", "2"}, {"3", "4"}};
    EXPECT_EQ(get_table, true_table);

    initialize(data_id);
}

TEST(ValueTableTest, SuccessGetEmptyTable)
{
    const std::string data_id = "SuccessGetEmptyTable";
    initialize(data_id);

    const std::string data = R"({"value":[])"
                             R"(,"meta":{"piece_id":0,"schema":[]}})";
    fs::create_directories("/db/share/" + data_id);
    auto ofs = std::ofstream("/db/share/" + data_id + "/0");
    ofs << data;
    ofs.close();

    auto table = qmpc::ComputationToDb::ValueTable(data_id);
    auto get_table = table.getTable();

    std::vector<std::vector<std::string>> true_table = {};
    EXPECT_EQ(get_table, true_table);

    initialize(data_id);
}

TEST(ValueTableTest, SuccessPieceGetTable)
{
    const std::string data_id = "SuccessPieceGetTable";
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

    auto table = qmpc::ComputationToDb::ValueTable(data_id);
    auto get_table = table.getTable();

    std::vector<std::vector<std::string>> true_table = {
        {"1", "2"}, {"3", "4"}, {"5", "6"}, {"7", "8"}, {"9", "10"}};
    EXPECT_EQ(get_table, true_table);

    initialize(data_id);
}

TEST(ValueTableTest, SuccessGetColumn)
{
    const std::string data_id = "SuccessGetColumn";
    initialize(data_id);

    const std::string data = R"({"value":[["1","2"],["3","4"]])"
                             R"(,"meta":{"piece_id":0,"schema":["attr1","attr2"]}})";
    fs::create_directories("/db/share/" + data_id);
    auto ofs = std::ofstream("/db/share/" + data_id + "/0");
    ofs << data;
    ofs.close();

    auto table = qmpc::ComputationToDb::ValueTable(data_id);

    std::vector<std::vector<std::string>> true_column = {{"1", "3"}, {"2", "4"}};
    for (int column_number = 0; column_number < 2; ++column_number)
    {
        auto column = table.getColumn(column_number);
        EXPECT_EQ(column, true_column[column_number]);
    }

    initialize(data_id);
}

TEST(ComputationToDbTest, SuccessGetSchema)
{
    const std::string data_id = "SuccessReadSchemaTest";
    initialize(data_id);

    const std::string data = R"({"value":[["1","2"],["3","4"]])"
                             R"(,"meta":{"piece_id":0,"schema":[)"
                             R"({"name": "attr1", "type": 0},)"
                             R"({"name": "attr2", "type": 0})"
                             R"(]}})";
    ;
    fs::create_directories("/db/share/" + data_id);
    auto ofs = std::ofstream("/db/share/" + data_id + "/0");
    ofs << data;
    ofs.close();

    auto table = qmpc::ComputationToDb::ValueTable(data_id);
    auto get_schema = table.getSchemas();

    using SchemaType = qmpc::ComputationToDb::SchemaType;
    std::vector<SchemaType> true_schema = {
        SchemaType("attr1", pb_common_types::ShareValueTypeEnum::SHARE_VALUE_TYPE_UNSPECIFIED),
        SchemaType("attr2", pb_common_types::ShareValueTypeEnum::SHARE_VALUE_TYPE_UNSPECIFIED)};

    EXPECT_EQ(get_schema, true_schema);

    initialize(data_id);
}
