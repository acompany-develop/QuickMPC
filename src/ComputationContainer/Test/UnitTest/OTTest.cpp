#include "gtest/gtest.h"

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

using namespace boost::multiprecision::literals;
namespace asio = boost::asio;
using asio::ip::tcp;
namespace bm = boost::multiprecision;

// 2^256 - 189
// //bm::cpp_int prime = 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF43_cppui;
// bm::cpp_int prime = 1000000007;
// bm::cpp_int g = 20000;
// bm::cpp_int h = 300001;
constexpr int64_t prime = 1000000007ll;
constexpr int64_t g = 2;
constexpr int64_t h = 3;

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

template <typename T>
void send(T &msg, const std::string &port, const std::string &ip)
{
    asio::io_service io_service;
    tcp::socket socket(io_service);

    tcp::resolver resolver(io_service);
    tcp::resolver::query query(ip, port);
    auto endpoint = resolver.resolve(query)->endpoint();
    // 接続
    socket.connect(endpoint);
    // std::cout << endpoint.address().to_string() << std::endl;
    //  メッセージ送信
    asio::streambuf receive_buffer;
    boost::system::error_code error;
    asio::write(socket, asio::buffer(msg), error);

    if (error)
    {
        std::cout << "send failed: " << error.message() << std::endl;
    }
    else
    {
        // std::cout << msg << std::endl;
        //  asio::read(socket, receive_buffer, asio::transfer_all(), error);
        //  const char *data = asio::buffer_cast<const char *>(receive_buffer.data());
        //  std::cout << data << std::endl;
    }
}

template <typename T>
T recv(size_t size, int port)
{
    asio::io_service io_service;

    tcp::acceptor acc(io_service, tcp::endpoint(tcp::v4(), port));
    tcp::socket socket(io_service);

    // 接続待機
    acc.accept(socket);

    // メッセージ受信
    T msg;
    msg.resize(size);
    asio::streambuf receive_buffer;
    boost::system::error_code error;
    asio::read(socket, asio::buffer(msg), error);
    if (error && error != asio::error::eof)
    {
        std::cout << "receive failed: " << error.message() << std::endl;
    }
    else
    {
        // const char *data = asio::buffer_cast<const char *>(receive_buffer.data());
        //  asio::write(socket, asio::buffer(k), error);
        return msg;
    }
}
TEST(OTTest, boost)
{
    std::thread th(
        [&]()
        {
            send("ping", "31400", "computation_container1");
            auto vec = recv<std::vector<int>>(32, 31401);
            for (auto a : vec)
            {
                std::cout << "recv vec is " << a << std::endl;
            }
            send("ping3", "31402", "computation_container1");
        }
    );
    std::cout << recv<std::string>(10, 31400) << std::endl;
    std::vector<int> vec = {11, 12, 13, 14, 15};
    vec.resize(32);
    send(vec, "31401", "computation_container1");
    std::cout << recv<std::string>(10, 31402) << std::endl;
    th.join();
}
TEST(OTTest, ot1)
{
    // std::cout << "p is " << prime << std::endl;
    // int s = 4;
    // int r = 3;

    // bm::cpp_int beta = powm(g, s, prime);
    // std::cout << beta << std::endl;
    // bm::cpp_int beta2 = powm(h, r, prime);
    // std::cout << beta2 << std::endl;
    // (beta *= beta2) %= prime;
    // bm::cpp_int B = static_cast<bm::int256_t>(beta);
    // std::cout << "b is " << B << std::endl;
    // std::thread th([&]() { send(B.str()); });
    // auto data = recv();
    // std::vector<int> x = {10, 11, 12, 13};
    // std::vector<int> k(x.size());
    // std::random_device rnd;  // 非決定的な乱数生成器を生成
    // std::mt19937 mt(rnd());  //  メルセンヌ・ツイスタの32ビット版、引数は初期シード値
    // std::uniform_int_distribution<> rand100(0, 100000);  // [0, 99] 範囲の一様乱数
    // std::vector<std::pair<bm::cpp_int, bm::cpp_int>> ab;
    // int i = 1;
    // for (auto &a : k)
    // {
    //     a = rand100(mt);
    //     std::cout << "rand is " << a << std::endl;
    //     std::cout << "gk is " << powm(g, a, prime) << std::endl;
    //     std::cout << "hinv exp " << a * i * (prime - 1) << std::endl;
    //     std::cout << "h_inv is " << powm(h, a * i * (prime -2), prime) << std::endl;
    //     auto gk = powm(g, a, prime);
    //     auto h_ = powm(h, i, prime);
    //     //std::cout << "h inv integer is " << boost::integer::mod_inverse(h_,prime) << std::endl;
    //     auto h_inv = powm(h_, a * i * (prime - 2), prime);
    //     auto b_ = powm(B, a, prime);
    //     std::cout << "b- " << b_ << std::endl;
    //     bm::cpp_int x_{x[i-1]};
    //     auto xb = x_ * b_;
    //     //std::cout << "xb is " << xb << std::endl;
    //     auto xbh = xb * h_inv % prime;
    //     if (i == 3)
    //     {
    //         auto a_inv = powm(gk, prime - 2 + r, prime);
    //         std::cout << "a_inv is " << a_inv << std::endl;
    //         auto ans = a_inv * xbh;
    //         //std::cout << "ans is " << ans << std::endl;
    //         // % prime;
    //         //std::cout << "answer : " << ans % prime << std::endl;
    //     }
    //     // ab.emplace_back(gk, xbh);
    //     // std::cout << ab[i].first << " " << ab[i].second << std::endl;
    //     i++;
    // }
    // for (int i = 0; i < x.size(); ++i)
    // {
    //     // auto gk = powm(g, ki, prime);
    //     // auto h_ = powm(h, i, prime);
    //     // auto h_inv = powm(h_, ki * i + prime - 1, prime);
    //     // auto b_ = powm(b, ki, prime);
    //     // bm::cpp_int x_ = x[i];
    //     // x_ *= b_;
    //     // x_ *= h_inv;
    //     // ab[i] = {gk, static_cast<bm::int512_t>(x_)};
    //     // std::cout << ab[i].first << " " << ab[i].second << std::endl;
    // }

    // // auto [a_tmp, b_tmp] = ab[3];
    // // auto a_inv = powm(a_tmp, 3, prime);
    // // bm::cpp_int ans = b_tmp;
    // // ans *= a_inv;
    // // ans %= prime;
    // // std::cout << "answer : " << ans << std::endl;
    // std::cout << data << std::endl;
    // th.join();
}
TEST(OTTest, OT4)
{
    int64_t r = 3;

    std::vector<int> x = {10, 11, 12, 13, 14};
    std::random_device rnd;  // 非決定的な乱数生成器を生成
    std::mt19937 mt(rnd());
    std::uniform_int_distribution<> rand(0, 100000);
    int64_t s = rand(mt);
    auto beta = pow(g, s, prime);   // 4
    auto beta2 = pow(h, r, prime);  // 27
    (beta *= beta2) %= prime;
    std::cout << beta << std::endl;

    std::vector<int64_t> k(x.size());
    int i = 1;
    std::vector<std::pair<int64_t, int64_t>> ab;
    for (auto &a : k)
    {
        a = rand(mt);
        auto gk = pow(g, a, prime);
        auto hinv = pow(h, i * a * (prime - 2), prime);
        auto bx = (x[i - 1] * pow(beta, a, prime)) % prime;
        auto bxh = (bx * hinv) % prime;
        ab.emplace_back(gk, bxh);
        i++;
    }
    for (auto &&[a, b] : ab)
    {
        auto as_inv = pow(a, s * (prime - 2), prime);
        std::cout << (pow(a, s, prime) * as_inv) % prime << std::endl;
        std::cout << "ans is " << (b * as_inv) % prime << std::endl;
    }
}
TEST(OTTest, ot5)
{
    int64_t r = 3;

    std::vector<int64_t> x = {1, 0, 1, 1, 0, 1};

    std::random_device rnd;  // 非決定的な乱数生成器を生成
    std::mt19937 mt(rnd());
    std::uniform_int_distribution<> rand(0, 100000);
    std::uniform_int_distribution<> rand_ast(1, 100000);
    int64_t s = rand(mt);
    int64_t s_dash = rand(mt);
    int64_t r_dash = rand(mt);

    std::cout << "s , s_dash, r_dash" << s << " " << s_dash << " " << r_dash << std::endl;
    // recvier
    auto beta = ((pow(g, s, prime) % prime) * pow(h, r, prime)) % prime;

    auto beta_dash = ((pow(g, s_dash, prime) % prime) * pow(h, r_dash, prime)) % prime;

    // send beta ,beta_dash
    // sender
    int64_t c = 20;
    // send c
    // reciver
    int64_t z1 = (s + (s_dash * c) % prime) % prime;
    int64_t z2 = (r + (r_dash * c) % prime) % prime;
    std::cout << "z1 is " << z1 << std::endl;
    std::cout << "z2 is " << z2 << std::endl;
    // send z , z2
    // sender
    auto gz = pow(g, z1, prime) % prime;
    auto hz = pow(h, z2, prime) % prime;
    std::cout << "gz , hz " << gz << " " << hz << std::endl;
    int64_t left = (beta * (pow(beta_dash, c, prime) % prime)) % prime;
    int64_t right = (gz * hz) % prime;
    std::cout << "left is " << left << std::endl;
    std::cout << "right is " << right << std::endl;
    if (left != right)
    {
        std::cout << "abort " << std::endl;
        return;
    }

    std::vector<int64_t> k(x.size());
    int i = 1;
    std::vector<std::pair<int64_t, int64_t>> ab;
    for (auto &a : k)
    {
        a = rand(mt);
        auto gk = pow(g, a, prime);
        auto hinv = pow(h, i * a * (prime - 2), prime);
        auto bx = (x[i - 1] * pow(beta, a, prime)) % prime;
        auto bxh = (bx * hinv) % prime;
        ab.emplace_back(gk, bxh);
        i++;
    }
    for (auto &&[a, b] : ab)
    {
        auto as_inv = pow(a, s * (prime - 2), prime);
        std::cout << (pow(a, s, prime) * as_inv) % prime << std::endl;
        std::cout << "ans is " << (b * as_inv) % prime << std::endl;
    }
}
TEST(OTTest, ot7)
{
    constexpr size_t size = 32;
    constexpr size_t N = prime;
    int64_t r = 4;
    std::thread th(
        [&]()
        {
            const auto clock_start = std::chrono::system_clock::now();
            std::random_device rnd;  // 非決定的な乱数生成器を生成
            std::mt19937 mt(rnd());
            std::uniform_int_distribution<> rand(0, 100000);
            std::uniform_int_distribution<> rand_ast(1, 100000);
            int64_t s = rand(mt);
            auto beta = pow(g, s, prime);   // 4
            auto beta2 = pow(h, r, prime);  // 27
            (beta *= beta2) %= prime;
            std::cout << beta << std::endl;
            std::vector<int64_t> b;
            b.emplace_back(beta);
            send(b, "30000", "computation_container1");
            auto ab = recv<std::vector<std::pair<bm::cpp_int, int64_t>>>(size, 30001);
            for (int i = 0; i < size; ++i)
            {
                bm::cpp_int e = ab[i].first;
                int64_t a = ab[i].second;
                bm::cpp_int as = pow(a, s, prime);
                auto hash = sha256(as.str() + std::to_string(i + 1));
                // std::cout << "recv " << i << " hash is " << hash << std::endl;
                std::string hash_int;
                boost::algorithm::hex(hash, std::back_inserter(hash_int));
                bm::cpp_int value(hash_int);
                auto xr = e ^ value;
                if (i == (r - 1)) std::cout << "ans is " << xr << std::endl;
            }
            const auto clock_end = std::chrono::system_clock::now();
            const auto elapsed_time_ms =
                std::chrono::duration_cast<std::chrono::microseconds>(clock_end - clock_start)
                    .count();
            std::cout << "recv Elapsed time =" << elapsed_time_ms << std::endl;
        }
    );

    const auto clock_start = std::chrono::system_clock::now();
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
    auto b = recv<std::vector<int64_t>>(1, 30000);
    int64_t beta = b.back();
    int64_t k = rand(mt);
    auto a = pow(g, k, prime);
    std::vector<std::pair<bm::cpp_int, int64_t>> ab;
    for (int i = 1; i <= size; ++i)
    {
        auto bk = pow(beta, k, prime);
        auto hinv = pow(h, i * k * (prime - 2), prime);
        bm::cpp_int bh = (bk * hinv) % prime;

        auto hash = sha256(bh.str() + std::to_string(i));
        // std::cout << "hash is " << i << " " << hash << std::endl;

        std::string hash_int;
        boost::algorithm::hex(hash, std::back_inserter(hash_int));
        bm::cpp_int value(hash_int);
        // std::cout << value << std::endl;

        auto ei = x[i - 1] ^ value;
        ab.emplace_back(ei, a);
    }
    send(ab, "30001", "computation_container1");

    const auto clock_end = std::chrono::system_clock::now();
    const auto elapsed_time_ms =
        std::chrono::duration_cast<std::chrono::microseconds>(clock_end - clock_start).count();
    std::cout << "sender Elapsed time =" << elapsed_time_ms << std::endl;
    th.join();
}
