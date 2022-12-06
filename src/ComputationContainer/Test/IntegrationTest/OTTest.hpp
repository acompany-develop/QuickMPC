#pragma once

#include <openssl/sha.h>
#include <sodium.h>
#include <boost/algorithm/hex.hpp>
#include <boost/asio.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/multiprecision/integer.hpp>
#include <chrono>
#include <functional>
#include <iostream>
#include <random>
#include <thread>
#include "ConfigParse/ConfigParse.hpp"
#include "Server/ComputationToComputationContainer/Server.hpp"
#include "Share/Share.hpp"
#include "gtest/gtest.h"

using namespace boost::multiprecision::literals;
namespace asio = boost::asio;
using asio::ip::tcp;
namespace bm = boost::multiprecision;

template <typename B, typename T, typename P>
B pow(B base, T a, P prime)
{
    B ret{1};
    while (a > 0)
    {
        if (a & 1) (ret *= base) %= prime;
        (base *= base) %= prime;
        a >>= 1;
    }
    return ret % prime;
}

std::string sha256(const std::string str)
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

template <typename T, typename Socket>
void send(Socket &&socket, T &msg)
{
    //  メッセージ送信
    asio::streambuf receive_buffer;
    boost::system::error_code error;
    asio::write(socket, asio::buffer(msg), asio::transfer_all(), error);

    if (error)
    {
        std::cout << "send failed: " << error.message() << std::endl;
    }
    else
    {
        std::cout << "send success" << std::endl;
        // std::cout << msg << std::endl;
        //  asio::read(socket, receive_buffer, asio::transfer_all(), error);
        //  const char *data = asio::buffer_cast<const char *>(receive_buffer.data());
        //  std::cout << data << std::endl;
    }
}

template <typename T>
T recv(tcp::socket &socket, size_t size)
{
    // メッセージ受信
    T msg;
    msg.resize(size);
    asio::streambuf receive_buffer;
    boost::system::error_code error;
    asio::read(socket, asio::buffer(msg), asio::transfer_all(), error);
    if (error && error != asio::error::eof)
    {
        std::cout << "receive failed: " << error.message() << std::endl;
    }
    else
    {
        std::cout << "recv success" << std::endl;
        for (auto a : msg)
        {
            std::cout << "recv message is " << a << std::endl;
        }
        // const char *data = asio::buffer_cast<const char *>(receive_buffer.data());
        //  asio::write(socket, asio::buffer(k), error);
        return msg;
    }
}
TEST(OTTest, ot1)
{
    const auto clock_start = std::chrono::system_clock::now();
    Config *conf = Config::getInstance();
    int pt_id = conf->party_id;
    constexpr int64_t prime = 1000000007ll;
    constexpr int64_t g = 2;
    constexpr int64_t h = 3;
    constexpr size_t size = 32;
    constexpr size_t N = prime;

    qmpc::Share::Share<int64_t> first;
    std::vector<qmpc::Share::Share<bm::cpp_int>> second(size + 1);

    qmpc::Share::AddressId id = first.getId();
    std::vector<qmpc::Share::AddressId> ids_list(size + 1);  // 複数シェアのidリスト
    for (int i = 0; i <= size; ++i)
    {
        ids_list[i] = second[i].getId();
    }
    if (pt_id == 1)
    {
        // std::string ip = "computation_container2";
        // std::string port = "30000";
        // asio::io_service io_service;
        // tcp::socket socket(io_service);

        // tcp::resolver resolver(io_service);
        // tcp::resolver::query query(ip, port);
        // auto endpoint = resolver.resolve(query)->endpoint();

        // std::cout << endpoint.address().to_string() << std::endl;

        // // 接続
        // socket.connect(endpoint);
        // std::cout << endpoint.address().to_string() << std::endl;

        int64_t r = 5;

        // 1st
        std::random_device rnd;  // 非決定的な乱数生成器を生成
        std::mt19937 mt(rnd());
        std::uniform_int_distribution<> rand(0, 100000);
        std::uniform_int_distribution<> rand_ast(1, 100000);
        int64_t s = rand(mt);
        auto beta = pow(g, s, prime);  // 4
        std::cout << "beta is " << beta << std::endl;
        auto beta2 = pow(h, r, prime);  // 27
        std::cout << "beta2 is " << beta2 << std::endl;
        (beta *= beta2) %= prime;
        std::cout << "b is " << beta << std::endl;
        std::vector<int64_t> b;
        b.emplace_back(beta);
        first = beta;
        send(first, 2);
        // send(socket, b);

        // 2nd
        auto server = qmpc::ComputationToComputation::Server::getServer();

        std::vector<std::string> ab = server->getShares(2, ids_list, size + 1);
        // auto ab = recv<std::vector<bm::cpp_int>>(socket, size + 1);
        std::cout << "ab size is " << ab.size() << std::endl;
        int64_t a = static_cast<int64_t>(std::stol(ab.back()));
        ab.pop_back();
        std::cout << "a is " << a << std::endl;
        std::cout << "ab size is " << ab.size() << std::endl;
        int i = 0;
        for (auto estr : ab)
        {
            bm::cpp_int e{estr};
            // std::cout << "e is " << e << std::endl;
            bm::cpp_int as = pow(a, s, prime);
            // std::cout << "a is " << e << " b is " << as << std::endl;
            auto hash = sha256(as.str() + std::to_string(i));
            // std::cout << "recv " << i << " hash is " << hash << std::endl;
            std::string hash_int;
            boost::algorithm::hex(hash, std::back_inserter(hash_int));
            bm::cpp_int value(hash_int);
            auto xr = e ^ value;
            if (i == (r - 1)) std::cout << "ans is " << xr << std::endl;
            i++;
        }
    }
    else if (pt_id == 2)
    {
        // asio::io_service io_service;

        // tcp::acceptor acc(io_service, tcp::endpoint(tcp::v4(), 30000));
        // tcp::socket socket(io_service);

        // std::cout << "socket init complete" << std::endl;
        // // 接続待機
        // acc.accept(socket);

        // std::vector<int64_t> x = {11, 12, 13, 14, 15, 16};
        std::vector<int64_t> x;
        for (int i = 0; i < 32; ++i)
        {
            x.emplace_back(1 << i);
        }
        // std::rotate(x.begin(), x.begin() + 4, x.end());
        std::random_device rnd;  // 非決定的な乱数生成器を生成
        std::mt19937 mt(rnd());
        std::uniform_int_distribution<> rand(0, 100000);
        std::uniform_int_distribution<> rand_ast(1, 100000);
        // auto b = recv<std::vector<int64_t>>(socket, 1);
        // 1nd
        auto server = qmpc::ComputationToComputation::Server::getServer();

        int64_t beta = std::stol(server->getShare(1, first.getId()));
        // int64_t beta = b.back();
        int64_t k = rand(mt);
        auto a = pow(g, k, prime);
        std::vector<bm::cpp_int> ab;
        for (int i = 0; i < size; ++i)
        {
            auto bk = pow(beta, k, prime);
            auto hinv = pow(h, (i + 1) * k * (prime - 2), prime);
            bm::cpp_int bh = (bk * hinv) % prime;

            auto hash = sha256(bh.str() + std::to_string(i));
            // std::cout << "hash is " << i << " " << hash << std::endl;

            std::string hash_int;
            boost::algorithm::hex(hash, std::back_inserter(hash_int));
            bm::cpp_int value(hash_int);
            // std::cout << value << std::endl;

            bm::cpp_int ei = value ^ x[i];
            // std::cout << "ei is " << ei << std::endl;
            ab.emplace_back(ei);
        }
        ab.emplace_back(a);
        for (int i = 0; i <= size; ++i)
        {
            second[i] = ab[i];
        }
        send(second, 1);
    }

    const auto clock_end = std::chrono::system_clock::now();
    const auto elapsed_time_ms =
        std::chrono::duration_cast<std::chrono::milliseconds>(clock_end - clock_start).count();
    std::cout << "sender Elapsed time =" << elapsed_time_ms << std::endl;
}