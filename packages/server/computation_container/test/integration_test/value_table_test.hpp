#include <experimental/filesystem>
#include <string>
#include <vector>

#include "client/computation_to_db/value_table.hpp"
#include "gtest/gtest.h"
namespace fs = std::experimental::filesystem;

// TODO: ValueTableのコンストラクタが変更されたので全部修正する

/****************** Test用の各データ ******************/
// DBを初期化する
auto initialize(const std::string& id)
{
    fs::remove_all("/Db/share/" + id);
    fs::remove_all("/Db/result/" + id);
}
const std::vector<std::vector<std::string>> table_data1{{"101", "1", "2"}, {"102", "3", "4"}};
const std::vector<std::string> schemas1{"id", "attr1", "attr2"};

const std::vector<std::vector<std::string>> table_data2{{"101", "5", "6"}, {"103", "7", "8"}};
const std::vector<std::string> schemas2{"id", "attr1", "attr3"};

const std::vector<std::vector<std::string>> table_data3{
    {"102", "9", "10", "11"}, {"103", "12", "13", "14"}};
const std::vector<std::string> schemas3{"id", "attr1", "attr3", "attr4"};

const std::vector<std::vector<std::string>> table_data4{
    {"103", "15", "16", "17"}, {"104", "18", "19", "20"}, {"105", "21", "22", "23"}};
const std::vector<std::string> schemas4{"id", "attr3", "attr4", "attr5"};

const std::vector<std::vector<std::string>> table_data5{{"101", "1", "0"}, {"102", "0", "4"}};
const std::vector<std::string> schemas5{"id", "attr1", "attr2"};

auto writeTable(
    const std::string& data_id,
    const std::vector<std::vector<std::string>>& table,
    const std::vector<std::string>& schema
)
{
    const int piece_id = 0;
    nlohmann::json data_json = {
        {"value", table}, {"meta", {{"piece_id", piece_id}, {"schema", schema}}}};
    const std::string data = data_json.dump();

    auto data_path = "/Db/share/" + data_id;
    fs::create_directories(data_path);
    std::ofstream ofs(data_path + "/" + std::to_string(piece_id));
    ofs << data;
    ofs.close();
    return data_id;
}

/********************** Test **********************/
TEST(ValueTableTest, vjoinTest)
{
    auto data_id1 = "vjoinTest1";
    auto data_id2 = "vjoinTest2";
    auto data_id3 = "vjoinTest3";
    auto data_id4 = "vjoinTest4";
    writeTable(data_id1, table_data1, schemas1);
    writeTable(data_id2, table_data2, schemas2);
    writeTable(data_id3, table_data3, schemas3);
    writeTable(data_id4, table_data4, schemas4);

    // table1 に table2 を縦結合した場合
    auto vt1 = qmpc::ComputationToDb::ValueTable(data_id1);
    auto vt2 = qmpc::ComputationToDb::ValueTable(data_id2);
    auto join_table1 = vt1.vjoin(vt2, 1, 1);

    const std::vector<std::vector<std::string>> table_data_1v2{
        {"101", "1"}, {"102", "3"}, {"103", "7"}};
    const std::vector<std::string> schemas_1v2{"id", "attr1"};
    EXPECT_EQ(join_table1.getTable(), table_data_1v2);
    EXPECT_EQ(join_table1.getSchemas(), schemas_1v2);

    // // table3 に table4 を縦結合した場合
    // auto vt3 = qmpc::ComputationToDb::ValueTable(data_id3);
    // auto vt4 = qmpc::ComputationToDb::ValueTable(data_id4);
    // auto join_table2 = vt3.vjoin(vt4, 1, 1);

    // const std::vector<std::vector<std::string>> table_data_3v4{
    //     {"102", "10", "11"}, {"103", "13", "14"}, {"104", "18", "19"}, {"105", "21", "22"}};
    // const std::vector<std::string> schemas_3v4{"id", "attr3", "attr4"};
    // EXPECT_EQ(join_table2.getTable(), table_data_3v4);
    // EXPECT_EQ(join_table2.getSchemas(), schemas_3v4);

    // initialize(data_id1);
    // initialize(data_id2);
    // initialize(data_id3);
    // initialize(data_id4);
}

// TEST(ValueTableTest, hjoinTest)
// {
//     // table1 に table2 を横結合した場合
//     const std::vector<std::vector<std::string>> table_data_1h2{{"101", "1", "2", "5", "6"}};
//     const std::vector<std::string> schemas_1h2{"id", "attr1", "attr2", "attr1", "attr3"};
//     const auto table_1h2 = qmpc::ComputationToDb::ValueTable(table_data_1h2, schemas_1h2);
//     EXPECT_EQ(table_1h2, table1.hjoin(table2, 1, 1));

//     // table3 に table4 を横結合した場合
//     const std::vector<std::vector<std::string>> table_data_3h4{
//         {"103", "12", "13", "14", "15", "16", "17"}};
//     const std::vector<std::string> schemas_3h4{
//         "id", "attr1", "attr3", "attr4", "attr3", "attr4", "attr5"};
//     const auto table_3h4 = qmpc::ComputationToDb::ValueTable(table_data_3h4, schemas_3h4);
//     EXPECT_EQ(table_3h4, table3.hjoin(table4, 1, 1));
// }

// TEST(ValueTableTest, vhjoinTest)
// {
//     // vjoin(table1,table2) と table3 を横結合した場合
//     const std::vector<std::vector<std::string>> table_data_1v2h3{
//         {"102", "3", "9", "10", "11"}, {"103", "7", "12", "13", "14"}};
//     const std::vector<std::string> schemas_1v2h3{"id", "attr1", "attr1", "attr3", "attr4"};
//     const auto table_1v2h3 = qmpc::ComputationToDb::ValueTable(table_data_1v2h3, schemas_1v2h3);
//     EXPECT_EQ(table_1v2h3, table1.vjoin(table2, 1, 1).hjoin(table3, 1, 1));
// }

// TEST(ValueTableTest, hvjoinTest)
// {
//     // hjoin(table1,table2) と table3 を縦結合した場合
//     const std::vector<std::vector<std::string>> table_data_1h2v3{
//         {"101", "1", "5", "6"}, {"102", "9", "9", "10"}, {"103", "12", "12", "13"}};
//     const std::vector<std::string> schemas_1h2v3{"id", "attr1", "attr1", "attr3"};
//     const auto table_1h2v3 = qmpc::ComputationToDb::ValueTable(table_data_1h2v3, schemas_1h2v3);
//     auto t = table1.hjoin(table2, 1, 1).vjoin(table3, 1, 1);
//     EXPECT_EQ(table_1h2v3, table1.hjoin(table2, 1, 1).vjoin(table3, 1, 1));
// }

// TEST(ValueTableTest, vjoinColumnTest)
// {
//     // 2列目を指定して table1 に table5 を縦結合した場合
//     const std::vector<std::vector<std::string>> table_data_1v5_2{
//         {"101", "1", "2"}, {"102", "3", "4"}, {"102", "0", "4"}};
//     const std::vector<std::string> schemas_1v5_2{"id", "attr1", "attr2"};
//     const auto table_1v5_2 = qmpc::ComputationToDb::ValueTable(table_data_1v5_2, schemas_1v5_2);
//     EXPECT_EQ(table_1v5_2, table1.vjoin(table5, 2, 2));

//     // 3列目を指定して table1 に table5 を縦結合した場合
//     const std::vector<std::vector<std::string>> table_data_1v5_3{
//         {"101", "1", "2"}, {"102", "3", "4"}, {"101", "1", "0"}};
//     const std::vector<std::string> schemas_1v5_3{"id", "attr1", "attr2"};
//     const auto table_1v5_3 = qmpc::ComputationToDb::ValueTable(table_data_1v5_3, schemas_1v5_3);
//     EXPECT_EQ(table_1v5_3, table1.vjoin(table5, 3, 3));
// }

// TEST(ValueTableTest, hjoinColumnTest)
// {
//     // 2列目を指定して table1 に table5 を横結合した場合
//     const std::vector<std::vector<std::string>> table_data_1h5_2{{"101", "1", "2", "101", "0"}};
//     const std::vector<std::string> schemas_1h5_2{"id", "attr1", "attr2", "id", "attr2"};
//     const auto table_1h5_2 = qmpc::ComputationToDb::ValueTable(table_data_1h5_2, schemas_1h5_2);
//     EXPECT_EQ(table_1h5_2, table1.hjoin(table5, 2, 2));

//     // 3列目を指定して table1 に table5 を横結合した場合
//     const std::vector<std::vector<std::string>> table_data_1h5_3{{"102", "3", "4", "102", "0"}};
//     const std::vector<std::string> schemas_1h5_3{"id", "attr1", "attr2", "id", "attr1"};
//     const auto table_1h5_3 = qmpc::ComputationToDb::ValueTable(table_data_1h5_3, schemas_1h5_3);
//     EXPECT_EQ(table_1h5_3, table1.hjoin(table5, 3, 3));
// }

// TEST(ValueTableTest, hjoinShareTest)
// {
//     // table1 に table2 を横結合した場合
//     EXPECT_EQ(table1.hjoinShare(table2, 1, 1), table1.hjoin(table2, 1, 1));

//     // table3 に table4 を横結合した場合
//     EXPECT_EQ(table3.hjoinShare(table4, 1, 1), table3.hjoin(table4, 1, 1));
// }

// TEST(ValueTableTest, hjoinShareColumnTest)
// {
//     auto table_tmp1 = table_data1;
//     auto table_tmp5 = table_data5;

//     // 2列目を指定して table1 に table5 を横結合した場合
//     // IDがソートされてる必要があるのでソートしておく
//     std::sort(
//         table_tmp1.begin(),
//         table_tmp1.end(),
//         [](const auto& a, const auto& b) { return a[1] < b[1]; }
//     );
//     std::sort(
//         table_tmp5.begin(),
//         table_tmp5.end(),
//         [](const auto& a, const auto& b) { return a[1] < b[1]; }
//     );
//     const auto table1_sorted1 = qmpc::ComputationToDb::ValueTable(table_tmp1, schemas1);
//     const auto table5_sorted1 = qmpc::ComputationToDb::ValueTable(table_tmp5, schemas5);
//     EXPECT_EQ(
//         table1_sorted1.hjoinShare(table5_sorted1, 2, 2), table1_sorted1.hjoin(table5_sorted1, 2,
//         2)
//     );

//     // 3列目を指定して table1 に table5 を横結合した場合
//     std::sort(
//         table_tmp1.begin(),
//         table_tmp1.end(),
//         [](const auto& a, const auto& b) { return a[2] < b[2]; }
//     );
//     std::sort(
//         table_tmp5.begin(),
//         table_tmp5.end(),
//         [](const auto& a, const auto& b) { return a[2] < b[2]; }
//     );
//     const auto table1_sorted2 = qmpc::ComputationToDb::ValueTable(table_tmp1, schemas1);
//     const auto table5_sorted2 = qmpc::ComputationToDb::ValueTable(table_tmp5, schemas5);
//     EXPECT_EQ(
//         table1_sorted2.hjoinShare(table5_sorted2, 3, 3), table1_sorted2.hjoin(table5_sorted2, 3,
//         3)
//     );
// }
