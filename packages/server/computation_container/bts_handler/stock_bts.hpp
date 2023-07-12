#pragma once

#include <queue>
#include <vector>

#include "client/computation_to_bts/client.hpp"
#include "job.hpp"

namespace qmpc::BtsHandler
{
template <typename BTSJob>
class StockBTS
{
    const std::size_t lowest_request_amount = 100000;

    using Result = typename BTSJob::result_type;
    thread_local static inline std::queue<Result> stock;

    // stock に n 個追加
    void requestBTS(const std::size_t amount)
    {
        auto client = ComputationToBts::Client::getInstance();
        std::vector<Result> ret = client->readRequest<BTSJob>(amount);

        for (const Result &x : ret)
        {
            stock.push(x);
        }
    }

public:
    static std::shared_ptr<StockBTS<BTSJob>> getInstance()
    {
        static auto instance = std::make_shared<StockBTS<BTSJob>>();
        return instance;
    }
    std::vector<Result> get(std::size_t amount = 1)
    {
        if (stock.size() < amount)
        {
            requestBTS(std::max(amount - stock.size(), lowest_request_amount));
        }

        std::vector<Result> ret(amount);
        for (Result &x : ret)
        {
            x = stock.front();
            stock.pop();
        }

        return ret;
    }
};

template <typename T>
using StockTriple = StockBTS<BTSJobType::Triple<T>>;

template <typename T>
using StockRandBit = StockBTS<BTSJobType::RandBit<T>>;
}  // namespace qmpc::BtsHandler
