#pragma once

#include <openssl/sha.h>
#include <sodium.h>
#include <bitset>
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
#include "Share/Compare.hpp"
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
        //  const char *data = asio::buffer_cast<const char
        //  *>(receive_buffer.data()); std::cout << data << std::endl;
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
        // const char *data = asio::buffer_cast<const char
        // *>(receive_buffer.data());
        //  asio::write(socket, asio::buffer(k), error);
        return msg;
    }
}
// TEST(OTTest, ot1)
// {
//     const auto clock_start = std::chrono::system_clock::now();
//     Config *conf = Config::getInstance();
//     int pt_id = conf->party_id;
//     constexpr int64_t prime = 1000000007ll;
//     constexpr int64_t g = 2;
//     constexpr int64_t h = 3;
//     constexpr size_t size = 32;
//     constexpr size_t N = prime;

//     qmpc::Share::Share<int64_t> first;
//     std::vector<qmpc::Share::Share<bm::cpp_int>> second(size + 1);

//     qmpc::Share::AddressId id = first.getId();
//     std::vector<qmpc::Share::AddressId> ids_list(size + 1);  //
//     複数シェアのidリスト for (int i = 0; i <= size; ++i)
//     {
//         ids_list[i] = second[i].getId();
//     }
//     std::cout << id << std::endl;
//     if (pt_id == 1)
//     {
//         // std::string ip = "computation_container2";
//         // std::string port = "30000";
//         // asio::io_service io_service;
//         // tcp::socket socket(io_service);

//         // tcp::resolver resolver(io_service);
//         // tcp::resolver::query query(ip, port);
//         // auto endpoint = resolver.resolve(query)->endpoint();

//         // std::cout << endpoint.address().to_string() << std::endl;

//         // // 接続
//         // socket.connect(endpoint);
//         // std::cout << endpoint.address().to_string() << std::endl;

//         int64_t r = 5;

//         // 1st
//         std::random_device rnd;  // 非決定的な乱数生成器を生成
//         std::mt19937 mt(rnd());
//         std::uniform_int_distribution<> rand(0, 100000);
//         std::uniform_int_distribution<> rand_ast(1, 100000);
//         int64_t s = rand(mt);
//         auto beta = pow(g, s, prime);  // 4
//         std::cout << "beta is " << beta << std::endl;
//         auto beta2 = pow(h, r, prime);  // 27
//         std::cout << "beta2 is " << beta2 << std::endl;
//         (beta *= beta2) %= prime;
//         std::cout << "b is " << beta << std::endl;
//         std::vector<int64_t> b;
//         b.emplace_back(beta);
//         first = beta;
//         send(first, 2);
//         // send(socket, b);

//         // 2nd
//         auto server = qmpc::ComputationToComputation::Server::getServer();

//         std::vector<std::string> ab = server->getShares(2, ids_list, size +
//         1);
//         // auto ab = recv<std::vector<bm::cpp_int>>(socket, size + 1);
//         std::cout << "ab size is " << ab.size() << std::endl;
//         int64_t a = static_cast<int64_t>(std::stol(ab.back()));
//         ab.pop_back();
//         std::cout << "a is " << a << std::endl;
//         std::cout << "ab size is " << ab.size() << std::endl;
//         int i = 0;
//         for (auto estr : ab)
//         {
//             bm::cpp_int e{estr};
//             // std::cout << "e is " << e << std::endl;
//             bm::cpp_int as = pow(a, s, prime);
//             // std::cout << "a is " << e << " b is " << as << std::endl;
//             auto hash = sha256(as.str() + std::to_string(i));
//             // std::cout << "recv " << i << " hash is " << hash << std::endl;
//             std::string hash_int;
//             boost::algorithm::hex(hash, std::back_inserter(hash_int));
//             bm::cpp_int value(hash_int);
//             auto xr = e ^ value;
//             if (i == (r - 1)) std::cout << "ans is " << xr << std::endl;
//             i++;
//         }
//     }
//     else if (pt_id == 2)
//     {
//         // asio::io_service io_service;

//         // tcp::acceptor acc(io_service, tcp::endpoint(tcp::v4(), 30000));
//         // tcp::socket socket(io_service);

//         // std::cout << "socket init complete" << std::endl;
//         // // 接続待機
//         // acc.accept(socket);

//         // std::vector<int64_t> x = {11, 12, 13, 14, 15, 16};
//         std::vector<int64_t> x;
//         for (int i = 0; i < 32; ++i)
//         {
//             x.emplace_back(1 << i);
//         }
//         // std::rotate(x.begin(), x.begin() + 4, x.end());
//         std::random_device rnd;  // 非決定的な乱数生成器を生成
//         std::mt19937 mt(rnd());
//         std::uniform_int_distribution<> rand(0, 100000);
//         std::uniform_int_distribution<> rand_ast(1, 100000);
//         // auto b = recv<std::vector<int64_t>>(socket, 1);
//         // 1nd
//         auto server = qmpc::ComputationToComputation::Server::getServer();

//         int64_t beta = std::stol(server->getShare(1, first.getId()));
//         // int64_t beta = b.back();
//         int64_t k = rand(mt);
//         auto a = pow(g, k, prime);
//         std::vector<bm::cpp_int> ab;
//         for (int i = 0; i < size; ++i)
//         {
//             auto bk = pow(beta, k, prime);
//             auto hinv = pow(h, (i + 1) * k * (prime - 2), prime);
//             bm::cpp_int bh = (bk * hinv) % prime;

//             auto hash = sha256(bh.str() + std::to_string(i));
//             // std::cout << "hash is " << i << " " << hash << std::endl;

//             std::string hash_int;
//             boost::algorithm::hex(hash, std::back_inserter(hash_int));
//             bm::cpp_int value(hash_int);
//             // std::cout << value << std::endl;

//             bm::cpp_int ei = value ^ x[i];
//             // std::cout << "ei is " << ei << std::endl;
//             ab.emplace_back(ei);
//         }
//         ab.emplace_back(a);
//         for (int i = 0; i <= size; ++i)
//         {
//             second[i] = ab[i];
//         }
//         send(second, 1);
//     }
//     std::cout << second[0].getId() << std::endl;

//     const auto clock_end = std::chrono::system_clock::now();
//     const auto elapsed_time_ms =
//         std::chrono::duration_cast<std::chrono::milliseconds>(clock_end -
//         clock_start).count();
//     std::cout << "Elapsed time =" << elapsed_time_ms << std::endl;
// }

TEST(OTTest, ot2)
{
    const auto clock_start = std::chrono::system_clock::now();
    size_t size = 32;
    qmpc::OT ot(size);
    Config *conf = Config::getInstance();
    int pt_id = conf->party_id;

    std::vector<std::vector<bm::cpp_int>> x(size, std::vector<bm::cpp_int>(size));
    int r = 15;
    if (pt_id == 1)
    {
        auto chosen = ot.recieve(2, r);
        for (auto a : chosen)
        {
            std::cout << a;
        }
        std::cout << std::endl;
    }
    else if (pt_id == 2)
    {
        for (int i = 0; i < size; ++i)
        {
            x[i][i] = 1ll;
        }
        ot.send(1, x);
    }

    // for (auto &&a : ot.second)
    // {
    //     std::cout << a.getId() << std::endl;
    // }
    const auto clock_end = std::chrono::system_clock::now();
    const auto elapsed_time_ms =
        std::chrono::duration_cast<std::chrono::milliseconds>(clock_end - clock_start).count();
    std::cout << "Elapsed time =" << elapsed_time_ms << std::endl;
}

// TEST(OTTest, unitvprep)
// {
//     auto rotate = [](unsigned int x, int r) { return x << r | x >> (32 - r); };
//     size_t N = 32;
//     qmpc::OT ot(N), ot2(N);
//     Config *conf = Config::getInstance();
//     int pt_id = conf->party_id;
//     int n_parties = conf->n_parties;
//     int r = RandGenerator::getInstance()->getRand<long long>(0, N - 1);
//     auto random_s = RandGenerator::getInstance()->getRandVec<long long>(1, 1 << N - 1, N - 1);

//     std::cout << pt_id << " party r is " << r << std::endl;
//     qmpc::Share::Share<int> rShare = r;
//     std::vector<qmpc::Share::Share<bm::cpp_int>> ret(N);
//     std::vector<qmpc::Share::Share<bm::cpp_int>> x_1_temp(N);
//     if (pt_id == 1)
//     {
//         // v[to] = data;
//         std::vector<std::vector<bm::cpp_int>> v(N, std::vector<bm::cpp_int>(N));
//         for (int i = 0; i < N; ++i)
//         {
//             auto ri = RandGenerator::getInstance()->getRandVec<long long>(1, 1 << N - 1, N);

//             for (int j = 0; j < N; ++j)
//             {
//                 v[i][j] = bm::cpp_int{ri[j]};
//             }
//         }
//         std::vector<bm::cpp_int> e(N);
//         e[r - 1] = 1ll;
//         v[0] = e;
//         std::vector<std::vector<bm::cpp_int>> x(N, std::vector<bm::cpp_int>(N));
//         for (int i = 0; i < N; ++i)
//         {
//             auto vi = v[0];
//             std::rotate(vi.begin(), vi.begin() + N - i - 1, vi.end());
//             std::vector<bm::cpp_int> xi(N);
//             for (int i = 0; i < N; ++i)
//             {
//                 xi[i] = vi[i] - v[1][i];
//             }
//             x[i] = xi;
//         }
//         for (int i = 0; i < N; ++i)
//         {
//             // std::cout << "v 1 is " << v[1][i] << std::endl;
//             x_1_temp[i] = v[1][i];
//         }
//         ot.send(2, x);
//         for (int i = 0; i < N; ++i)
//         {
//             auto vi = v[1];
//             std::rotate(vi.begin(), vi.begin() + N - i - 1, vi.end());
//             std::vector<bm::cpp_int> xi(N);
//             for (int i = 0; i < N; ++i)
//             {
//                 xi[i] = vi[i] - v[2][i];
//                 // std::cout << "ot x " << i << " is " << std::bitset<32>(xi[i]) <<
//                 // std::endl;
//             }
//             // std::cout << "ot x " << i << " is " << std::bitset<32>(xi) <<
//             // std::endl;
//             x[i] = xi;
//         }
//         ot.send(3, x);
//         for (int i = 0; i < N; ++i)
//         {
//             // std::cout << "v 2 is " << v[2][i] << std::endl;
//             ret[i] = v[2][i];
//         }
//         open(ret);
//         auto ret_rec = recons(ret);
//         int i = 0;
//         for (auto a : ret_rec)
//         {
//             std::cout << " index is " << i << " " << a << std::endl;
//             i++;
//         }
//     }
//     else if (pt_id == 2)
//     {
//         // v[to] = data;

//         std::vector<std::vector<bm::cpp_int>> v(N, std::vector<bm::cpp_int>(N));
//         for (int i = 0; i < N; ++i)
//         {
//             auto ri = RandGenerator::getInstance()->getRandVec<long long>(1, 1 << N - 1, N);

//             for (int j = 0; j < N; ++j)
//             {
//                 v[i][j] = bm::cpp_int{ri[j]};
//             }
//         }
//         auto rec = ot.recieve(1, r);
//         for (int i = 0; i < N; ++i)
//         {
//             v[1][i] = rec[i];
//             // std::cout << "party 2 rec is " << v[1][i] << std::endl;
//             x_1_temp[i] = v[1][i];
//         }

//         std::vector<std::vector<bm::cpp_int>> x(N, std::vector<bm::cpp_int>(N));
//         for (int i = 0; i < N; ++i)
//         {
//             auto vi = v[1];
//             std::rotate(vi.begin(), vi.begin() + N - i - 1, vi.end());
//             std::vector<bm::cpp_int> xi(N);
//             for (int i = 0; i < N; ++i)
//             {
//                 xi[i] = vi[i] - v[2][i];
//             }
//             // std::cout << "ot x " << i << " is " << std::bitset<32>(xi) <<
//             // std::endl;
//             x[i] = xi;
//         }
//         ot.send(3, x);
//         for (int i = 0; i < N; ++i)
//         {
//             // std::cout << "v 2 is " << v[2][i] << std::endl;
//             ret[i] = v[2][i];
//         }
//         open(ret);
//         auto ret_rec = recons(ret);
//         int i = 0;
//         for (auto a : ret_rec)
//         {
//             std::cout << " index is " << i << " " << a << std::endl;
//             i++;
//         }
//     }
//     else if (pt_id == 3)
//     {
//         // v[to] = data;

//         std::vector<std::vector<bm::cpp_int>> v(N, std::vector<bm::cpp_int>(N));
//         for (int i = 0; i < N; ++i)
//         {
//             auto ri = RandGenerator::getInstance()->getRandVec<long long>(1, 1 << N - 1, N);

//             for (int j = 0; j < N; ++j)
//             {
//                 v[i][j] = bm::cpp_int{ri[j]};
//             }
//         }
//         auto rec = ot.recieve(1, r);
//         auto rec2 = ot.recieve(2, r);
//         for (int i = 0; i < N; ++i)
//         {
//             ret[i] = rec[i] + rec2[i];
//         }
//         open(ret);
//         auto ret_rec = recons(ret);
//         int i = 0;
//         for (auto a : ret_rec)
//         {
//             std::cout << " index is " << i << " " << a << std::endl;
//             i++;
//         }
//     }
//     open(x_1_temp);
//     auto tmp_rec = recons(x_1_temp);
//     int i = 0;
//     for (auto &a : tmp_rec)
//     {
//         if (a == 1)
//         {
//             std::cout << "temp rec is " << i << std::endl;
//         }
//         i++;
//     }
//     open(rShare);
//     auto recR = recons(rShare);
//     // std::cout << "value is " << std::bitset<32>(rec) << std::endl;
//     std::cout << "random value is" << (recR % 32) << std::endl;
// }

TEST(OTTest, unitvpMulti)
{
    auto [r1, v1] = qmpc::Share::unitvPrep<32>();
    auto [r2, v2] = qmpc::Share::unitvPrep<32>();
    auto [r3, v3] = qmpc::Share::unitvPrep<32>();
    // open(r1);
    // open(r2);
    // open(r3);
    // open(v1);
    // open(v2);
    // open(v3);
    // auto rec_r1 = recons(r1);
    // auto rec_r2 = recons(r2);
    // auto rec_r3 = recons(r3);
    // auto rec_v1 = recons(v1);
    // auto rec_v2 = recons(v2);
    // auto rec_v3 = recons(v3);
    // std::cout << " random is" << rec_r1 << " " << rec_r2 << " " << rec_r3 << std::endl;
    // std::cout << "value is " << std::endl;
    // for (auto a : rec_v1)
    // {
    //     std::cout << " v is " << a << std::endl;
    // }

    // for (auto a : rec_v2)
    // {
    //     std::cout << " v is " << a << std::endl;
    // }
    // for (auto a : rec_v3)
    // {
    //     std::cout << " v is " << a << std::endl;
    // }
}

TEST(OTTest, unitv)
{
    qmpc::Share::Share<bm::cpp_int> index{1};
    auto v = qmpc::Share::unitv(index);
    open(v);
    auto v_rec = recons(v);
    int i = 0;
    for (auto &a : v_rec)
    {
        std::cout << "index " << i << " v value is " << a << std::endl;
        i++;
    }
}

TEST(OTTest, compare)
{
    qmpc::Share::Share<bm::cpp_int> x{5};
    qmpc::Share::Share<bm::cpp_int> y{5};
    qmpc::Share::Share<bm::cpp_int> z{4};
    auto ok = qmpc::Share::equality1(x, y);
    // open(ok);
    // auto rec = recons(ok);
    // std::cout << "rec is " << rec << std::endl;

    auto ng = qmpc::Share::equality1(x, z);
    // open(ng);
    // auto rec_ng = recons(ng);
    // std::cout << "rec_ng is " << rec_ng << std::endl;
}

TEST(OTTest, expand)
{
    int x = 54321;
    std::cout << std::bitset<32>(x) << std::endl;
    auto x_dash = qmpc::Share::expand(x);
    for (auto &&a : x_dash)
    {
        std::cout << a << std::endl;
    }
}
TEST(OTTest, compareFull)
{
    qmpc::Share::Share<bm::cpp_int> x{5};
    qmpc::Share::Share<bm::cpp_int> y{5};
    qmpc::Share::Share<bm::cpp_int> z{4};
    auto ok = qmpc::Share::equality(x, y);
    // open(ok);
    // auto rec = recons(ok);
    // std::cout << "rec is " << rec << std::endl;

    auto ng = qmpc::Share::equality(x, z);
    // open(ng);
    // auto rec_ng = recons(ng);
    // std::cout << "rec_ng is " << rec_ng << std::endl;
}

TEST(OTTest, compareMulti)
{
    qmpc::Share::Share<bm::cpp_int> x{5};
    qmpc::Share::Share<bm::cpp_int> y{5};
    qmpc::Share::Share<bm::cpp_int> z{4};
    // open(ok);
    // auto rec = recons(ok);
    // std::cout << "rec is " << rec << std::endl;
    for (int i = 0; i < 100; ++i)
    {
        auto ok = qmpc::Share::equality(x, y);
    }
    // auto ng = qmpc::Share::equality(x, z);
    //  open(ng);
    //  auto rec_ng = recons(ng);
    //  std::cout << "rec_ng is " << rec_ng << std::endl;
}

// TEST(OTTest, RBS)
// {
//     size_t N = 1000;
//     {
//         const auto clock_start = std::chrono::system_clock::now();
//         //[[maybe_unused]] auto [_, v] = qmpc::Share::unitvPrep<2>();
//         auto random = qmpc::Share::getRandBitShare<qmpc::Utils::FixedPointImpl<>>(N);

//         const auto clock_end = std::chrono::system_clock::now();
//         const auto elapsed_time_ms =
//             std::chrono::duration_cast<std::chrono::milliseconds>(clock_end -
//             clock_start).count();
//         std::cout << "Elapsed time grb =" << elapsed_time_ms << std::endl;
//     }
//     {
//         const auto clock_start = std::chrono::system_clock::now();
//         std::vector<qmpc::Share::Share<bm::cpp_int>> ret;
//         for (int i = 0; i < N; ++i)
//         {
//             auto [r, v1] = qmpc::Share::unitvPrep<16>();
//             ret.emplace_back(v1[0]);
//         }
//         const auto clock_end = std::chrono::system_clock::now();
//         const auto elapsed_time_ms =
//             std::chrono::duration_cast<std::chrono::milliseconds>(clock_end -
//             clock_start).count();
//         std::cout << "Elapsed time unitv grb =" << elapsed_time_ms << std::endl;
//         open(ret);
//         auto rec_v = recons(ret);
//         int i = 0;
//         for (auto a : rec_v)
//         {
//             if (a == 1)
//             {
//                 std::cout << " v is " << i << std::endl;
//             }
//             i++;
//         }
//     }
// }

TEST(OTTest, OTe)
{
    size_t N = 32;
    size_t array_size = 30000;
    qmpc::OTe ot(N, array_size);
    Config *conf = Config::getInstance();
    int pt_id = conf->party_id;

    std::vector<std::vector<std::vector<bm::cpp_int>>> x(
        array_size, std::vector<std::vector<bm::cpp_int>>(N, std::vector<bm::cpp_int>(N))
    );
    for (int arr = 0; arr < array_size; ++arr)
    {
        for (int i = 0; i < N; ++i)
        {
            x[arr][i][i] = 1ll;
        }
    }
    std::vector<int> r(array_size, 15);
    r[0] = 1;

    const auto clock_start = std::chrono::system_clock::now();
    if (pt_id == 1)
    {
        auto chosen = ot.recieve(2, r);
        // for (auto a : chosen)
        // {
        //     for (auto b : a)
        //     {
        //         std::cout << b;
        //     }
        //     std::cout << std::endl;
        // }
    }
    else if (pt_id == 2)
    {
        ot.send(1, x);
    }

    // for (auto &&a : ot.second)
    // {
    //     std::cout << a.getId() << std::endl;
    // }
    const auto clock_end = std::chrono::system_clock::now();
    const auto elapsed_time_ms =
        std::chrono::duration_cast<std::chrono::milliseconds>(clock_end - clock_start).count();
    std::cout << "Elapsed time =" << elapsed_time_ms << std::endl;
}