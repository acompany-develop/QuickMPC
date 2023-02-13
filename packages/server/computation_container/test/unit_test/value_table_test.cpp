#include <experimental/filesystem>
#include <fstream>

#include "client/computation_to_db/value_table_new.hpp"
#include "gtest/gtest.h"

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
