#include <experimental/filesystem>

#include "Client/ComputationToDb/Client.hpp"
#include "gtest/gtest.h"
namespace fs = std::experimental::filesystem;

// shareの取り出し
// qmpc::ComputationToDbGate::ValueTable Client::readShare(const std::string &data_id);
TEST(ComputationToDbTest, SuccessReadShareTest) {}
TEST(ComputationToDbTest, SuccessReadSharePieceTest) {}
TEST(ComputationToDbTest, SuccessReadShareLargeTest) {}

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
