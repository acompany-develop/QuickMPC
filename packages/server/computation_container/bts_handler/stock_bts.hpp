#pragma once

#include <queue>
#include <vector>

#include "client/computation_to_bts/client.hpp"
#include "job.hpp"

namespace qmpc::BtsHandler
{
template <typename Job>
class StockBTS
{
    const std::size_t threshold = 100000, M = 100000;

    using Result = typename Job::result_type;
    std::queue<Result> stock;

    // stock に n 個追加
    void requestBTS(const std::size_t amount)
    {
        auto client = ComputationToBts::Client::getInstance();
        std::vector<Result> ret = client->readRequest<Job>(amount);

        for (const Result &x : ret)
        {
            stock.push(x);
        }
    }

public:
    static std::shared_ptr<StockBTS<Job>> getInstance()
    {
        static auto instance = std::make_shared<StockBTS<Job>>();
        return instance;
    }
    std::vector<Result> get(std::size_t amount = 1)
    {
        if (stock.size() < amount)
        {
            requestBTS(std::max(amount - stock.size(), M));
        }

        std::vector<Result> ret(amount);
        for (Result &x : ret)
        {
            x = stock.front();
            stock.pop();
        }

        // 要らないかも
        if (stock.size() < threshold)
        {
            requestBTS(M);
        }

        return ret;
    }
};

template <typename T>
using StockTriple = StockBTS<JobType::Triple<T>>;

template <typename T>
using StockRandBit = StockBTS<JobType::RandBit<T>>;
}  // namespace qmpc::BtsHandler
