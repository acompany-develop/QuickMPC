#include "client/computation_to_bts/client.hpp"
#include "gtest/gtest.h"
#include "logging/logger.hpp"
#include "nlohmann/json.hpp"

void sync()
{
    // partyで足並みを揃えるための関数
    std::vector<Share> fixed_triples;
    Share a(1);
    Share b(1);
    Share c(1);
    fixed_triples.emplace_back(Share(1));
    fixed_triples.emplace_back(Share(1));
    fixed_triples.emplace_back(Share(1));

    open(fixed_triples);
    auto _ = recons(fixed_triples);
}

void readTriplesTest(const unsigned int jobIdMax, const unsigned int amount)
{
    // triple read Test
    auto cc_to_bts = qmpc::ComputationToBts::Client::getInstance();
    for (unsigned int jobId = 1; jobId <= jobIdMax; jobId++)
    {
        QMPC_LOG_INFO("jobId[{}]: ...", jobId);
        auto triples = cc_to_bts->readTriples<FixedPoint>(jobId, amount);
        EXPECT_EQ(triples.size(), amount);
        // TODO: Party間で足並みを揃えてa*b=cのチェック

        std::vector<Share> fixed_triples;
        for (unsigned int i = 0; i < amount; i++)
        {
            Share a(std::get<0>(triples[i]));
            Share b(std::get<1>(triples[i]));
            Share c(std::get<2>(triples[i]));
            fixed_triples.emplace_back(a);
            fixed_triples.emplace_back(b);
            fixed_triples.emplace_back(c);
        }
        open(fixed_triples);
        auto triple_rec = recons(fixed_triples);
        for (unsigned int i = 0; i < amount * 3; i += 3)
        {
            auto a_rec = triple_rec[i];
            auto b_rec = triple_rec[i + 1];
            auto c_rec = triple_rec[i + 2];
            EXPECT_EQ(a_rec * b_rec, c_rec);
        }
        QMPC_LOG_INFO("end");
    }
}

TEST(ComputationToBtsTest, ReadTriplesTest10_10000) { readTriplesTest(10, 10000); }

TEST(ComputationToBtsTest, InitTripleStoreTest)
{
    const unsigned int jobIdMax = 5;
    const unsigned int amount = 1000;
    Config *conf = Config::getInstance();
    auto cc_to_bts = qmpc::ComputationToBts::Client::getInstance();

    if (conf->party_id == 1)
    {
        for (unsigned int jobId = 1; jobId <= jobIdMax; jobId++)
        {
            auto triples = cc_to_bts->readTriples<FixedPoint>(jobId, amount);
        }

        // BTSの初期化
        cc_to_bts->initTripleStore();
    }

    // partyで足並みを揃えるため
    sync();

    readTriplesTest(jobIdMax, amount);
}

TEST(ComputationToBtsTest, DeleteJobIdTripleTest)
{
    const unsigned int jobIdMax = 5;
    const unsigned int amount = 1000;
    Config *conf = Config::getInstance();
    auto cc_to_bts = qmpc::ComputationToBts::Client::getInstance();

    if (conf->party_id == 1)
    {
        for (unsigned int jobId = 1; jobId <= jobIdMax; jobId++)
        {
            auto triples = cc_to_bts->readTriples<FixedPoint>(jobId, amount);
        }

        // 各jobIdに紐付いたTripleを削除する
        for (unsigned int jobId = 1; jobId <= jobIdMax; jobId++)
        {
            cc_to_bts->deleteJobIdTriple(jobId);
        }
    }

    // partyで足並みを揃えるため
    sync();

    readTriplesTest(jobIdMax, amount);
}

// TODO: 異なる引数で連続でテストする場合はParty間で足並みを揃える処理が必要

// TEST(ComputationToBtsTest, ReadTriplesTest1_1) { readTriplesTest(1, 1); }
// TEST(ComputationToBtsTest, ReadTriplesTest100_1) { readTriplesTest(100, 1); }
// TEST(ComputationToBtsTest, ReadTriplesTest10000_1) { readTriplesTest(10000,
// 1); } TEST(ComputationToBtsTest, ReadTriplesTest1_100) { readTriplesTest(1,
// 100); } TEST(ComputationToBtsTest, ReadTriplesTest100_100) {
// readTriplesTest(100, 100); } TEST(ComputationToBtsTest,
// ReadTriplesTest10000_100) { readTriplesTest(10000, 100); }
// TEST(ComputationToBtsTest, ReadTriplesTest1_10000) { readTriplesTest(1,
// 10000); } TEST(ComputationToBtsTest, ReadTriplesTest100_10000) {
// readTriplesTest(100, 10000); } TEST(ComputationToBtsTest,
// ReadTriplesTest10000_10000) { readTriplesTest(10000, 10000); }
