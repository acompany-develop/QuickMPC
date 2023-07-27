#pragma once

#include <boost/multiprecision/cpp_int.hpp>
#include <exception>
#include <iterator>
#include <stdexcept>
#include <string>
#include <thread>
#include <type_traits>
#include <vector>

#include "address_id.hpp"
#include "config_parse/config_parse.hpp"
#include "fixed_point/fixed_point.hpp"
#include "server/computation_to_computation_container/server.hpp"
namespace qmpc::Share
{
template <typename SV>
class Share;

template <typename SV>
void send(const Share<SV> &share, const int &pt_id)
{
    Config *conf = Config::getInstance();
    auto client = ComputationToComputation::Server::getServer()->getClient(pt_id);
    std::vector<SV> str_value = {share.getVal()};
    std::vector<qmpc::Share::AddressId> share_id = {share.getId()};
    client->exchangeShares(str_value, share_id, 1, conf->party_id);
}

template <typename SV>
void send(const std::vector<Share<SV>> &shares, const int &pt_id)
{
    // pt_idで指定されたパーティにシェアの値を送信する
    Config *conf = Config::getInstance();
    auto client = ComputationToComputation::Server::getServer()->getClient(pt_id);

    size_t length = std::size(shares);
    std::vector<SV> str_values(length);
    std::vector<qmpc::Share::AddressId> share_ids(length);
    for (unsigned int i = 0; i < length; i++)
    {
        str_values[i] = shares[i].getVal();
        share_ids[i] = shares[i].getId();
    }
    client->exchangeShares(str_values, share_ids, length, conf->party_id);
}

template <class SV>
void send(const SV &share_value, const AddressId &share_id, int pt_id)
{
    Config *conf = Config::getInstance();
    auto client = ComputationToComputation::Server::getServer()->getClient(pt_id);
    client->exchangeShare(share_value, share_id, conf->party_id);
}

// T : Share<SV> || vector<Share<SV>>
template <typename T>
void open(const T &share)
{
    Config *conf = Config::getInstance();
    std::exception_ptr ep;

    std::vector<std::thread> threads;
    // 全パーティにシェアの値を送信する
    for (int pt_id = 1; pt_id <= conf->n_parties; pt_id++)
    {
        if (pt_id == conf->party_id)
        {
            continue;
        }
        threads.emplace_back((
            [=, &share, &ep]()
            {
                try
                {
                    send(share, pt_id);
                }
                catch (...)
                {
                    ep = std::current_exception();
                }
            }
        ));
    }

    for (auto &th : threads)
    {
        th.join();
    }
    if (ep)
    {
        std::rethrow_exception(ep);
    }
}

using CtoCShare = computationtocomputation::Shares_Share;

template <typename SV>
auto recons(const Share<SV> &share)
{
    Config *conf = Config::getInstance();
    // シェア保有者が使うrecons関数
    auto server = ComputationToComputation::Server::getServer();
    // 単一
    using Result = std::conditional_t<std::is_same_v<SV, bool>, int, SV>;
    Result ret{};
    for (int pt_id = 1; pt_id <= conf->n_parties; pt_id++)
    {
        if (pt_id == conf->party_id)
        {
            ret += share.getVal();
        }
        else
        {
            ret += server->getShare<SV>(pt_id, share.getId());
        }
    }
    return ret;
}
template <typename SV>
auto recons(const std::vector<Share<SV>> &share)
{
    Config *conf = Config::getInstance();
    // シェア保有者が使うrecons関数
    auto server = ComputationToComputation::Server::getServer();
    // 一括部分
    size_t length = std::size(share);
    using Result = std::conditional_t<std::is_same_v<SV, bool>, int, SV>;
    std::vector<Result> ret(length);
    for (int pt_id = 1; pt_id <= conf->n_parties; pt_id++)
    {
        std::vector<qmpc::Share::AddressId> ids_list(length);  // 複数シェアのidリスト
        for (unsigned int i = 0; i < length; i++)
        {
            ids_list[i] = share[i].getId();
        }

        if (pt_id == conf->party_id)
        {
            for (unsigned int i = 0; i < length; i++)
            {
                ret[i] += share[i].getVal();
            }
        }
        else
        {
            std::vector<SV> values = server->getShares<SV>(pt_id, ids_list);
            for (unsigned int i = 0; i < length; i++)
            {
                ret[i] += values[i];
            }
        }
    }

    return ret;
}

// T : Share<SV> || vector<Share<SV>>
template <typename T>
auto open_and_recons(const T &share)
{
    open(share);
    return recons(share);
}
}  // namespace qmpc::Share
