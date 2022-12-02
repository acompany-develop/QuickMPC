#include "gtest/gtest.h"

#include <emp-ot/emp-ot.h>
#include <emp-tool/emp-tool.h>
#include <openssl/sha.h>
#include <sodium.h>
#include <boost/algorithm/hex.hpp>
#include <boost/asio.hpp>
#include <boost/integer/mod_inverse.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/multiprecision/integer.hpp>
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
int64_t prime = 1000000007ll;
int64_t g = 2;
int64_t h = 3;

template <typename T>
T pow(T base, T a, T prime)
{
    T ret{1};
    while (a > 0)
    {
        if (a & 1) (ret *= base) %= prime;
        (base *= base) %= prime;
        a >>= 1;
    }
    return ret % prime;
}

string sha256(const string str)
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
    std::cout << recv<string>(10, 31400) << std::endl;
    std::vector<int> vec = {11, 12, 13, 14, 15};
    vec.resize(32);
    send(vec, "31401", "computation_container1");
    std::cout << recv<string>(10, 31402) << std::endl;
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
    int64_t r = 3;

    std::vector<int64_t> x = {11, 12, 13, 14, 15, 16};

    std::random_device rnd;  // 非決定的な乱数生成器を生成
    std::mt19937 mt(rnd());
    std::uniform_int_distribution<> rand(0, 100000);
    std::uniform_int_distribution<> rand_ast(1, 100000);
    int64_t s = rand(mt);
    auto beta = pow(g, s, prime);   // 4
    auto beta2 = pow(h, r, prime);  // 27
    (beta *= beta2) %= prime;
    std::cout << beta << std::endl;

    int64_t k = rand(mt);
    auto a = pow(g, k, prime);
    std::vector<std::pair<bm::cpp_int, int64_t>> ab;
    for (int i = 1; i <= x.size(); ++i)
    {
        auto bk = pow(beta, k, prime);
        auto hinv = pow(h, i * k * (prime - 2), prime);
        auto bh = (bk * hinv) % prime;

        auto hash = sha256(std::to_string(bh) + std::to_string(i));
        std::cout << "hash is " << hash << std::endl;

        std::string hash_int;
        boost::algorithm::hex(hash, std::back_inserter(hash_int));
        bm::cpp_int value(hash_int);
        std::cout << value << std::endl;

        auto ei = x[i - 1] ^ value;
        ab.emplace_back(ei, a);
    }

    for (int i = 0; i < x.size(); ++i)
    {
        auto as = pow(a, s, prime);
        auto hash = sha256(std::to_string(as) + std::to_string(i + 1));
        // std::cout << "hash is " << hash << std::endl;
        std::string hash_int;
        boost::algorithm::hex(hash, std::back_inserter(hash_int));
        bm::cpp_int value(hash_int);
        auto xr = ab[i].first ^ value;
        std::cout << "ans is " << xr << std::endl;
    }
}
using namespace emp;

template <typename T>
double test_ot(T *ot, NetIO *io, int party, int64_t length)
{
    block *b0 = new block[length], *b1 = new block[length], *r = new block[length];
    PRG prg(fix_key);
    prg.random_block(b0, length);
    prg.random_block(b1, length);
    bool *b = new bool[length];
    PRG prg2;
    prg2.random_bool(b, length);

    auto start = clock_start();
    if (party == ALICE)
    {
        ot->send(b0, b1, length);
    }
    else
    {
        ot->recv(r, b, length);
    }
    io->flush();
    long long t = time_from(start);
    if (party == BOB)
    {
        for (int64_t i = 0; i < length; ++i)
        {
            if (b[i])
            {
                if (!cmpBlock(&r[i], &b1[i], 1))
                {
                    std::cout << i << "\n";
                    error("wrong!\n");
                }
            }
            else
            {
                if (!cmpBlock(&r[i], &b0[i], 1))
                {
                    std::cout << i << "\n";
                    error("wrong!\n");
                }
            }
        }
    }
    std::cout << "Tests passed.\t";
    delete[] b0;
    delete[] b1;
    delete[] r;
    delete[] b;
    return t;
}

template <typename T>
double test_cot(T *ot, NetIO *io, int party, int64_t length)
{
    block *b0 = new block[length], *r = new block[length];
    bool *b = new bool[length];
    block delta;
    PRG prg;
    prg.random_block(&delta, 1);
    prg.random_bool(b, length);

    io->sync();
    auto start = clock_start();
    if (party == ALICE)
    {
        ot->send_cot(b0, length);
        delta = ot->Delta;
    }
    else
    {
        ot->recv_cot(r, b, length);
    }
    io->flush();
    long long t = time_from(start);
    if (party == ALICE)
    {
        io->send_block(&delta, 1);
        io->send_block(b0, length);
    }
    else if (party == BOB)
    {
        io->recv_block(&delta, 1);
        io->recv_block(b0, length);
        for (int64_t i = 0; i < length; ++i)
        {
            block b1 = b0[i] ^ delta;
            if (b[i])
            {
                if (!cmpBlock(&r[i], &b1, 1)) error("COT failed!");
            }
            else
            {
                if (!cmpBlock(&r[i], &b0[i], 1)) error("COT failed!");
            }
        }
    }
    std::cout << "Tests passed.\t";
    io->flush();
    delete[] b0;
    delete[] r;
    delete[] b;
    return t;
}

template <typename T>
double test_rot(T *ot, NetIO *io, int party, int64_t length)
{
    block *b0 = new block[length], *r = new block[length];
    block *b1 = new block[length];
    bool *b = new bool[length];
    PRG prg;
    prg.random_bool(b, length);

    io->sync();
    auto start = clock_start();
    if (party == ALICE)
    {
        ot->send_rot(b0, b1, length);
    }
    else
    {
        ot->recv_rot(r, b, length);
    }
    io->flush();
    long long t = time_from(start);
    if (party == ALICE)
    {
        io->send_block(b0, length);
        io->send_block(b1, length);
    }
    else if (party == BOB)
    {
        io->recv_block(b0, length);
        io->recv_block(b1, length);
        for (int64_t i = 0; i < length; ++i)
        {
            if (b[i])
                assert(cmpBlock(&r[i], &b1[i], 1));
            else
                assert(cmpBlock(&r[i], &b0[i], 1));
        }
    }
    std::cout << "Tests passed.\t";
    io->flush();
    delete[] b0;
    delete[] b1;
    delete[] r;
    delete[] b;
    return t;
}

template <typename T>
double test_rcot(T *ot, NetIO *io, int party, int64_t length, bool inplace)
{
    block *b = nullptr;
    PRG prg;

    io->sync();
    auto start = clock_start();
    int64_t mem_size;
    if (!inplace)
    {
        mem_size = length;
        b = new block[length];

        // The RCOTs will be generated in the internal buffer
        // then be copied to the user buffer
        ot->rcot(b, length);
    }
    else
    {
        // Call byte_memory_need_inplace() to get the buffer size needed
        mem_size = ot->byte_memory_need_inplace((uint64_t)length);
        b = new block[mem_size];

        // The RCOTs will be generated directly to this buffer
        ot->rcot_inplace(b, mem_size);
    }
    long long t = time_from(start);
    io->sync();
    if (party == ALICE)
    {
        io->send_block(&ot->Delta, 1);
        io->send_block(b, mem_size);
    }
    else if (party == BOB)
    {
        block ch[2];
        ch[0] = zero_block;
        block *b0 = new block[mem_size];
        io->recv_block(ch + 1, 1);
        io->recv_block(b0, mem_size);
        for (int64_t i = 0; i < mem_size; ++i)
        {
            b[i] = b[i] ^ ch[getLSB(b[i])];
        }
        if (!cmpBlock(b, b0, mem_size)) error("RCOT failed");
        delete[] b0;
    }
    std::cout << "Tests passed.\t";
    delete[] b;
    return t;
}

int port, party;
const static int threads = 2;

void test_ferret(int party, NetIO *ios[threads], int64_t num_ot)
{
    using namespace std;
    auto start = clock_start();
    FerretCOT<NetIO> *ferretcot = new FerretCOT<NetIO>(party, threads, ios, true, true, ferret_b13);
    double timeused = time_from(start);
    std::cout << party << "\tsetup\t" << timeused / 1000 << "ms" << std::endl;

    // RCOT
    // The RCOTs will be generated at internal memory, and copied to user buffer
    int64_t num = 1 << num_ot;
    cout << "Active FERRET RCOT\t"
         << double(num) / test_rcot<FerretCOT<NetIO>>(ferretcot, ios[0], party, num, false) * 1e6
         << " OTps" << endl;

    // RCOT inplace
    // The RCOTs will be generated at user buffer
    // Get the buffer size needed by calling byte_memory_need_inplace()
    uint64_t batch_size = ferretcot->ot_limit;
    cout << "Active FERRET RCOT inplace\t"
         << double(batch_size)
                / test_rcot<FerretCOT<NetIO>>(ferretcot, ios[0], party, batch_size, true) * 1e6
         << " OTps" << endl;
    delete ferretcot;
}
// TEST(OTTest, ot2)
// {
//     port = 30000;
//     party = BOB;
//     NetIO *ios[threads];
//     for (int i = 0; i < threads; ++i) ios[i] = new NetIO("127.0.0.1", port + i);

//     int64_t length = 24;
//     std::thread th([&]() { test_ferret(party, ios, length); });

//     NetIO *alice[threads];
//     for (int i = 0; i < threads; ++i) alice[i] = new NetIO(nullptr, port + i);
//     test_ferret(ALICE, alice, length);

//     th.join();
//     for (int i = 0; i < threads; ++i)
//     {
//         delete ios[i];
//         delete alice[i];
//     }
// }