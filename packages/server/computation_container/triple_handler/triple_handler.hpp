#pragma once

#include <queue>
#include <tuple>

#include "client/computation_to_bts/client.hpp"
#include "fixed_point/fixed_point.hpp"
#include "share/address_id.hpp"

namespace qmpc::TripleHandler
{

template <typename T>
using Triple = std::tuple<T, T, T>;

class TripleHandler
{
    thread_local static inline std::queue<Triple<std::string>> triple_queue_fp;
    thread_local static inline std::queue<Triple<std::string>> triple_queue_float;

private:
    // 1Jobで必要なトリプルを下回るとBTS周りでいろいろ死ぬので，でかめに設定
    inline static unsigned int threshold =
        100000;  // 残ったトリプルの数が threshold を下回ったら補充する

public:
    static TripleHandler *getInstance()
    {
        static TripleHandler obj;
        return &obj;
    }

    template <typename TV>
    Triple<TV> takeOutTriple(std::queue<Triple<std::string>> &triple_queue)
    {
        // Tripleの残り数が閾値を下回ればTripleを閾値と同じ数だけ補充する
        if (triple_queue.size() < TripleHandler::threshold)
        {
            auto client = ComputationToBts::Client::getInstance();
            auto job_id = qmpc::Share::AddressId::getThreadJobId();
            for (const auto &triple : client->readTriples<TV>(job_id, TripleHandler::threshold))
            {
                triple_queue.push(triple);
            }
        }
        auto [a, b, c] = triple_queue.front();
        triple_queue.pop();
        if constexpr (std::is_integral_v<TV>)
        {
            return Triple<TV>(std::stoll(a), std::stoll(b), std::stoll(c));
        }
        else if constexpr (std::is_floating_point_v<TV>)
        {
            return Triple<TV>(std::stod(a), std::stod(b), std::stod(c));
        }
        else
        {
            return Triple<TV>(a, b, c);
        }
    }

    template <typename SV>
    auto getTriple(size_t needCount = 1)
    {
        using Result = Triple<SV>;
        std::vector<Result> ret;

        for (size_t count = 0; count < needCount; count++)
        {
            if constexpr (std::is_same_v<SV, float>)
            {
                ret.emplace_back(takeOutTriple<SV>(triple_queue_float));
            }
            else
            {
                ret.emplace_back(takeOutTriple<SV>(triple_queue_fp));
            }
        }
        return ret;
    }
};
}  // namespace qmpc::TripleHandler
