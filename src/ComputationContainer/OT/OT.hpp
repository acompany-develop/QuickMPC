#pragma once
#include <openssl/sha.h>
#include <sodium.h>
#include <boost/algorithm/hex.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/asio.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/multiprecision/integer.hpp>
#include <boost/serialization/vector.hpp>
#include <chrono>
#include <functional>
#include <iostream>
#include <random>
#include <thread>
#include "ConfigParse/ConfigParse.hpp"
#include "Group.hpp"
#include "Server/ComputationToComputationContainer/Server.hpp"
#include "Share/Share.hpp"
namespace qmpc
{
using namespace boost::multiprecision::literals;
namespace asio = boost::asio;
using asio::ip::tcp;
namespace bm = boost::multiprecision;
using Scala = bm::cpp_int;
template <typename B, typename T, typename P>
B pow(B base, T a, P prime)
{
    B ret{1};
    while (a > 0)
    {
        if (a & 1) (ret *= base) %= prime;
        (base *= base) %= prime;
        a = a / 2;
    }
    return ret % prime;
}

inline std::string sha256(const std::string str)
{
    using namespace std;
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, str.c_str(), str.size());
    SHA256_Final(hash, &sha256);
    stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        ss << hex << setw(2) << setfill('0') << (int)hash[i];
        // std::cout << setw(3) << setfill('0') << (int)hash[i] << std::endl;
    }
    return ss.str();
}

inline Scala make_hash(const Scala& x, int index)
{
    auto hash = sha256(x.str() + std::to_string(index));
    // std::cout << "hash is " << i << " " << hash << std::endl;

    std::string hash_int;
    boost::algorithm::hex(hash, std::back_inserter(hash_int));
    Scala value(hash_int);
    return value;
}
class OT
{
    inline static constexpr int64_t prime = 1000000007ll;
    inline static constexpr int64_t g = 2;
    inline static constexpr int64_t h = 3;

public:
    size_t size;
    qmpc::Share::Share<int64_t> first;
    qmpc::Share::Share<std::string> second;
    OT(size_t size_) : size(size_), first(), second() {}

    void send(int to_id, const std::vector<std::vector<bm::cpp_int>>& x)
    {
        // 1nd
        auto server = qmpc::ComputationToComputation::Server::getServer();
        int64_t beta = std::stol(server->getShare(to_id, first.getId()));
        bm::cpp_int k = RandGenerator::getInstance()->getRand<long long>(0, prime - 1);
        auto a = pow(g, k, prime);
        std::vector<std::vector<bm::cpp_int>> ab(size + 1);
        for (int i = 0; i < size; ++i)
        {
            bm::cpp_int k_dash = k * (i + 1) * (prime - 2);
            auto bk = pow(beta, k, prime);
            auto hinv = pow(h, k_dash, prime);
            bm::cpp_int bh = (bk * hinv) % prime;
            auto value = make_hash(bh, i);
            // std::cout << value << std::endl;

            for (int j = 0; j < size; ++j)
            {
                bm::cpp_int ei = value ^ x[i][j];
                // std::cout << "ei is " << ei << std::endl;
                ab[i].emplace_back(ei);
            }
        }
        ab[size].emplace_back(a);
        std::stringstream stream;
        boost::archive::binary_oarchive ar(stream);
        ar& ab;
        second = stream.str();
        qmpc::Share::send(second, to_id);
    }

    std::vector<bm::cpp_int> recieve(int from_id, int choise_id)
    {
        qmpc::Share::AddressId id = first.getId();
        qmpc::Share::AddressId id2 = second.getId();

        // 1st
        bm::cpp_int s = RandGenerator::getInstance()->getRand<long long>(0, prime - 1);
        auto beta = pow(g, s, prime);           // 4
        auto beta2 = pow(h, choise_id, prime);  // 27
        (beta *= beta2) %= prime;
        std::vector<int64_t> b;
        b.emplace_back(beta);
        first = beta;
        qmpc::Share::send(first, from_id);
        // send(socket, b);

        // 2nd
        auto server = qmpc::ComputationToComputation::Server::getServer();
        std::stringstream archive;
        archive << server->getShare(from_id, id2);
        std::vector<std::vector<bm::cpp_int>> ab;
        boost::archive::binary_iarchive ar(archive);
        ar& ab;
        // auto ab = recv<std::vector<bm::cpp_int>>(socket, size + 1);
        int64_t a = static_cast<int64_t>(ab.back().back());
        ab.pop_back();
        int i = 0;
        // for (auto cur : ab)
        // {
        //     for (auto estr : cur)
        //     {
        //         bm::cpp_int e{estr};
        //         std::cout << "e is " << e << std::endl;
        //         bm::cpp_int as = pow(a, s, prime);
        //         // std::cout << "a is " << e << " b is " << as << std::endl;
        //         auto value = make_hash(as,i);
        //         auto xr = e ^ value;
        //         if (i == (choise_id - 1)) std::cout << "ans is " << xr << std::endl;
        //     }
        //     i++;
        // }

        std::vector<bm::cpp_int> e = ab[choise_id - 1];
        bm::cpp_int as = pow(a, s, prime);
        auto value = make_hash(as, choise_id - 1);
        std::vector<bm::cpp_int> xr(size);
        for (int i = 0; i < size; ++i)
        {
            xr[i] = e[i] ^ value;
        }
        return xr;
    }
};
class OTe
{
    inline static constexpr int64_t prime = 1000000007ll;
    inline static constexpr int64_t g = 2;
    inline static constexpr int64_t h = 3;

public:
    size_t size;
    size_t array_size;
    std::vector<qmpc::Share::Share<Scala>> first;
    qmpc::Share::Share<std::string> second;
    OTe(size_t size_, size_t array_size)
        : size(size_), array_size(array_size), first(array_size), second()
    {
    }

    void send(int to_id, const std::vector<std::vector<std::vector<Scala>>>& x)
    {
        // 1nd
        auto server = qmpc::ComputationToComputation::Server::getServer();
        std::vector<std::string> beta_strings;
        std::vector<Scala> betas(array_size);
        std::vector<qmpc::Share::AddressId> ids(array_size);
        for (int i = 0; i < array_size; ++i)
        {
            ids[i] = first[i].getId();
        }
        beta_strings = server->getShares(to_id, ids, array_size);
        for (int i = 0; i < array_size; ++i)
        {
            betas[i] = Scala(beta_strings[i]);
        }
        // int64_t beta = std::stol(server->getShare(to_id, first.getId()));
        std::vector<Scala> ks =
            RandGenerator::getInstance()->getRandVec<Scala>(0, prime - 1, array_size);
        std::vector<Scala> a;
        for (int i = 0; i < array_size; ++i)
        {
            a.emplace_back(pow(g, ks[i], prime));
        }
        std::vector<std::vector<std::vector<Scala>>> ab(
            array_size, std::vector<std::vector<Scala>>(size)
        );
        for (int arr = 0; arr < array_size; ++arr)
        {
            for (int i = 0; i < size; ++i)
            {
                Scala k_dash = ks[arr] * (i + 1) * (prime - 2);
                auto bk = pow(betas[arr], ks[arr], prime);
                auto hinv = pow(h, k_dash, prime);
                Scala bh = (bk * hinv) % prime;
                auto value = make_hash(bh, i);
                // std::cout << value << std::endl;

                for (int j = 0; j < size; ++j)
                {
                    Scala ei = value ^ x[arr][i][j];
                    // std::cout << "ei is " << ei << std::endl;
                    ab[arr][i].emplace_back(ei);
                }
            }
        }
        std::stringstream stream;
        boost::archive::binary_oarchive ar(stream);
        ar& ab;
        ar& a;
        second = stream.str();
        qmpc::Share::send(second, to_id);
    }

    std::vector<std::vector<Scala>> recieve(int from_id, const std::vector<int>& choise_ids)
    {
        std::vector<qmpc::Share::AddressId> id1(array_size);
        qmpc::Share::AddressId id2;
        for (int i = 0; i < array_size; ++i)
        {
            id1[i] = first[i].getId();
        }
        id2 = second.getId();

        // 1st
        std::vector<Scala> s =
            RandGenerator::getInstance()->getRandVec<Scala>(0, prime - 1, array_size);

        for (int i = 0; i < array_size; ++i)
        {
            auto beta = pow(g, s[i], prime);            // 4
            auto beta2 = pow(h, choise_ids[i], prime);  // 27
            (beta *= beta2) %= prime;
            first[i] = beta;
        }

        qmpc::Share::send(first, from_id);
        // send(socket, b);

        // 2nd
        auto server = qmpc::ComputationToComputation::Server::getServer();
        std::stringstream archive;
        archive << server->getShare(from_id, id2);
        std::vector<std::vector<std::vector<Scala>>> ab;
        std::vector<Scala> a;
        boost::archive::binary_iarchive ar(archive);
        ar >> ab >> a;
        // auto ab = recv<std::vector<bm::cpp_int>>(socket, size + 1);
        int i = 0;
        // for (auto cur : ab)
        // {
        //     for (auto estr : cur)
        //     {
        //         bm::cpp_int e{estr};
        //         std::cout << "e is " << e << std::endl;
        //         bm::cpp_int as = pow(a, s, prime);
        //         // std::cout << "a is " << e << " b is " << as << std::endl;
        //         auto value = make_hash(as,i);
        //         auto xr = e ^ value;
        //         if (i == (choise_id - 1)) std::cout << "ans is " << xr << std::endl;
        //     }
        //     i++;
        // }
        std::vector<std::vector<Scala>> ret(array_size);
        for (int i = 0; i < array_size; ++i)
        {
            std::vector<Scala> e = ab[i][choise_ids[i] - 1];
            Scala as = pow(a[i], s[i], prime);
            auto value = make_hash(as, choise_ids[i] - 1);
            std::vector<Scala> xr(size);
            for (int j = 0; j < size; ++j)
            {
                xr[j] = e[j] ^ value;
            }
            ret[i] = xr;
        }
        return ret;
    }
};
}  // namespace qmpc