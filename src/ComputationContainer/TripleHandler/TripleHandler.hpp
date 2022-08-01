#pragma once

#include "FixedPoint/FixedPoint.hpp"
#include "PrimeField/PrimeField.hpp"
#include "Client/ComputationToBts/Client.hpp"
#include "Share/AddressId.hpp"

#include <tuple>
#include <queue>

namespace qmpc::TripleHandler
{
using TripleFp = std::tuple<::FixedPoint, ::FixedPoint, ::FixedPoint>;
using TriplePf = std::tuple<::PrimeField, ::PrimeField, ::PrimeField>;

class TripleHandler
{
    thread_local static inline std::queue<TripleFp> triple_fp_queue;
    thread_local static inline std::queue<TriplePf> triple_pf_queue;

private:
    // 1Jobで必要なトリプルを下回るとBTS周りでいろいろ死ぬので，でかめに設定
    inline static unsigned int threshold =
        100000;  // 残ったトリプルの数が threshold を下回ったら補充する

public:
    template <typename TV>
    TV convertStr2Triple(const auto &buf)
    {
        using SV = typename std::
            conditional<std::is_same_v<TV, TripleFp>, ::FixedPoint, ::PrimeField>::type;
        if constexpr (std::is_same_v<SV, ::PrimeField>)
        {
            // PrimeFieldで負の数が現状対応できない。
            // TODO FixedPointをかませばうまくいく？
            Config *conf = Config::getInstance();
            int n_parties = conf->n_parties;
            SV a(1);
            SV b(1);
            SV c(n_parties);
            return std::make_tuple(a, b, c);
        }
        else
        {
            SV a(std::get<0>(buf));
            SV b(std::get<1>(buf));
            SV c(std::get<2>(buf));
            return std::make_tuple(a, b, c);
        }
    }

    static TripleHandler *getInstance()
    {
        static TripleHandler obj;
        return &obj;
    }

    template <typename TV>
    TV takeOutTriple(std::queue<TV> &triple_queue)
    {
        // Tripleの残り数が閾値を下回ればTripleを閾値と同じ数だけ補充する
        if (triple_queue.size() < TripleHandler::threshold)
        {
            auto client = ComputationToBts::Client::getInstance();
            auto job_id = qmpc::Share::AddressId::getThreadJobId();
            for (const auto &triple : client->readTriples(job_id, TripleHandler::threshold))
            {
                triple_queue.push(convertStr2Triple<TV>(triple));
            }
        }
        auto triple = triple_queue.front();
        triple_queue.pop();
        return triple;
    }

    template <typename SV>
    auto getTriple(size_t needCount = 1)
    {
        using Result =
            typename std::conditional<std::is_same_v<SV, ::FixedPoint>, TripleFp, TriplePf>::type;
        std::vector<Result> ret;

        for (size_t count = 0; count < needCount; count++)
        {
            if constexpr (std::is_same_v<SV, ::FixedPoint>)
            {
                ret.emplace_back(takeOutTriple<Result>(this->triple_fp_queue));
            }
            else
            {
                ret.emplace_back(takeOutTriple<Result>(this->triple_pf_queue));
            }
        }
        return ret;
    }
};
}  // namespace qmpc::TripleHandler