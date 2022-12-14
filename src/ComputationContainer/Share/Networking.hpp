#pragma once

#include <boost/multiprecision/cpp_int.hpp>
#include <exception>
#include <iterator>
#include <stdexcept>
#include <string>
#include <thread>
#include <type_traits>
#include <vector>

#include "AddressId.hpp"
#include "ConfigParse/ConfigParse.hpp"
#include "FixedPoint/FixedPoint.hpp"
#include "Server/ComputationToComputationContainer/Server.hpp"
namespace qmpc::Share
{
using std::to_string;
template <typename T>
class Share;

template <typename>
struct is_share_vector : std::false_type
{
};
template <typename T, typename ALLOCATOR>
struct is_share_vector<std::vector<Share<T>, ALLOCATOR>> : std::true_type
{
};

template <typename>
struct is_share : std::false_type
{
};
template <typename T>
struct is_share<Share<T>> : std::true_type
{
};
template <typename T>
void send(const T &shares, const int &pt_id)
{
    // pt_idで指定されたパーティにシェアの値を送信する
    Config *conf = Config::getInstance();
    auto client = ComputationToComputation::Server::getServer()->getClient(pt_id);

    if constexpr (std::disjunction_v<std::is_array<T>, is_share_vector<T>>)
    {
        using SV = typename T::value_type::value_type;
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
    else if constexpr (is_share<T>::value)
    {
        // pt_idで指定されたパーティにシェアの値を送信する
        client->exchangeShare(shares.getVal(), shares.getId(), conf->party_id);
    }
}
template <class SV>
void send(const SV &share_value, const AddressId &share_id, int pt_id)
{
    Config *conf = Config::getInstance();
    auto client = ComputationToComputation::Server::getServer()->getClient(pt_id);
    client->exchangeShare(share_value, share_id, conf->party_id);
}
template <typename T>
void open(const T &share)
{
    Config *conf = Config::getInstance();
    std::exception_ptr ep;

    std::vector<std::thread> threads;
    // 全パーティにシェアの値を送信する
    for (int pt_id = 1; pt_id <= conf->n_parties; pt_id++)
    {
        if (pt_id != conf->party_id)
        {
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
template <typename T>
auto stosv(const std::string &str_value)
{
    if constexpr (std::is_same_v<T, bool>)
    {
        return std::stoi(str_value);
    }
    else if constexpr (std::is_integral_v<T>)
    {
        return std::stoi(str_value);
    }
    else if constexpr (std::is_floating_point_v<T>)
    {
        return std::stod(str_value);
    }
    else  // TODO: constructable or convertible
    {
        return T(str_value);
    }
}
template <typename T, std::enable_if_t<is_share<T>::value, std::nullptr_t> = nullptr>
auto recons(const T &share)
{
    Config *conf = Config::getInstance();
    // シェア保有者が使うrecons関数
    auto server = ComputationToComputation::Server::getServer();
    // 単一
    using Result = typename T::value_type;
    Result ret{};
    for (int pt_id = 1; pt_id <= conf->n_parties; pt_id++)
    {
        if (pt_id == conf->party_id)
        {
            if constexpr (std::is_same_v<Result, bool>)
            {
                ret ^= share.getVal();
            }
            else
            {
                ret += share.getVal();
            }
        }
        else
        {
            auto s = server->getShare(pt_id, share.getId());
            if constexpr (std::is_same_v<Result, bool>)
            {
                ret ^= Result{s == "1"};
            }
            else if constexpr (std::is_integral_v<Result>)
            {
                ret += Result{std::stoi(s)};
            }
            else if constexpr (std::is_floating_point_v<Result>)
            {
                ret += Result{std::stof(s)};
            }
            else
            {
                Result received_value{s};
                ret += received_value;
            }
        }
    }
    return ret;
}
template <typename T, std::enable_if_t<is_share_vector<T>::value, std::nullptr_t> = nullptr>
auto recons(const T &share)
{
    Config *conf = Config::getInstance();
    // シェア保有者が使うrecons関数
    auto server = ComputationToComputation::Server::getServer();
    // 一括部分
    using Result = typename T::value_type::value_type;
    size_t length = std::size(share);
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
                if constexpr (std::is_same_v<Result, bool>)
                {
                    ret[i] = ret[i] ^ share[i].getVal();
                }
                else
                    ret[i] += share[i].getVal();
            }
        }
        else
        {
            std::vector<std::string> values = server->getShares(pt_id, ids_list, length);
            for (unsigned int i = 0; i < length; i++)
            {
                if constexpr (std::is_same_v<Result, bool>)
                {
                    ret[i] = ret[i] ^ Result(std::stoi(values[i]));
                }
                else if constexpr (std::is_integral_v<Result>)
                {
                    ret[i] += Result(std::stoi(values[i]));
                }
                else if constexpr (std::is_floating_point_v<Result>)
                {
                    ret[i] += Result(std::stod(values[i]));
                }
                else
                {
                    ret[i] += Result(values[i]);
                }
            }
        }
    }

    return ret;
}
template <typename SV>
SV receive(int sp_id, AddressId address_id)
{
    ComputationToComputation::Server *server = ComputationToComputation::Server::getServer();
    if constexpr (std::is_constructible_v<SV, std::string>)
    {
        SV received_value(server->getShare(sp_id, address_id));
        return received_value;
    }
    else
    {
        SV received_value(std::stoi(server->getShare(sp_id, address_id)));
        return received_value;
    }
}
template <typename SV>
std::vector<SV> receive(int sp_id, const std::vector<AddressId> &address_ids)
{
    ComputationToComputation::Server *server = ComputationToComputation::Server::getServer();
    int n = address_ids.size();
    std::vector<SV> ret(n);
    auto shares = server->getShares(sp_id, address_ids, address_ids.size());
    for (int i = 0; i < n; ++i)
    {
        ret[i] = stosv<SV>(shares[i]);
    }
    return ret;
}
}  // namespace qmpc::Share
