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
#include "Server/ComputationToComputationContainer/Server.hpp"
#include "Share/Share.hpp"
namespace qmpc
{
using namespace boost::multiprecision::literals;
namespace asio = boost::asio;
using asio::ip::tcp;
namespace bm = boost::multiprecision;
class OT
{
    inline static constexpr int64_t prime = 1000000007ll;
    inline static constexpr int64_t g = 2;
    inline static constexpr int64_t h = 3;

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

public:
    size_t size;
    qmpc::Share::Share<int64_t> first;
    qmpc::Share::Share<std::string> second;
    OT(size_t size_) : size(size_), first(), second() {}

    void send(int to_id, const std::vector<int64_t>& x)
    {
        std::cout << first.getId() << std::endl;
        std::random_device rnd;  // 非決定的な乱数生成器を生成
        std::mt19937 mt(rnd());
        std::uniform_int_distribution<> rand(0, 100000);
        std::uniform_int_distribution<> rand_ast(1, 100000);
        // auto b = recv<std::vector<int64_t>>(socket, 1);
        // 1nd
        auto server = qmpc::ComputationToComputation::Server::getServer();
        int64_t beta = std::stol(server->getShare(to_id, first.getId()));
        // std::cout << "sender beta is " << beta << std::endl;
        //  int64_t beta = b.back();
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
        std::stringstream stream;
        boost::archive::binary_oarchive ar(stream);
        ar& ab;
        // for (int i = 0; i <= size; ++i)
        // {
        //     second[i] = ab[i];
        // }
        second = stream.str();
        qmpc::Share::send(second, to_id);
    }

    bm::cpp_int recieve(int from_id, int choise_id)
    {
        std::cout << first.getId() << std::endl;
        qmpc::Share::AddressId id = first.getId();
        qmpc::Share::AddressId id2 = second.getId();

        // 1st
        std::random_device rnd;  // 非決定的な乱数生成器を生成
        std::mt19937 mt(rnd());
        std::uniform_int_distribution<> rand(0, 100000);
        std::uniform_int_distribution<> rand_ast(1, 100000);

        int64_t s = rand(mt);
        auto beta = pow(g, s, prime);  // 4
        std::cout << "beta is " << beta << std::endl;
        auto beta2 = pow(h, choise_id, prime);  // 27
        std::cout << "beta2 is " << beta2 << std::endl;
        (beta *= beta2) %= prime;
        std::cout << "b is " << beta << std::endl;
        std::vector<int64_t> b;
        b.emplace_back(beta);
        first = beta;
        qmpc::Share::send(first, from_id);
        // send(socket, b);

        // 2nd
        auto server = qmpc::ComputationToComputation::Server::getServer();
        std::stringstream archive;
        archive << server->getShare(from_id, id2);
        std::vector<bm::cpp_int> ab;
        boost::archive::binary_iarchive ar(archive);
        ar& ab;
        // auto ab = recv<std::vector<bm::cpp_int>>(socket, size + 1);
        std::cout << "ab size is " << ab.size() << std::endl;
        int64_t a = static_cast<int64_t>(ab.back());
        ab.pop_back();
        // int i = 0;
        // for (auto estr : ab)
        // {
        //     bm::cpp_int e{estr};
        //     // std::cout << "e is " << e << std::endl;
        //     bm::cpp_int as = pow(a, s, prime);
        //     // std::cout << "a is " << e << " b is " << as << std::endl;
        //     auto hash = sha256(as.str() + std::to_string(i));
        //     // std::cout << "recv " << i << " hash is " << hash << std::endl;
        //     std::string hash_int;
        //     boost::algorithm::hex(hash, std::back_inserter(hash_int));
        //     bm::cpp_int value(hash_int);
        //     auto xr = e ^ value;
        //     if (i == (choise_id - 1)) std::cout << "ans is " << xr << std::endl;
        //     i++;
        // }

        bm::cpp_int e{ab[choise_id - 1]};
        bm::cpp_int as = pow(a, s, prime);
        auto hash = sha256(as.str() + std::to_string(choise_id - 1));
        // std::cout << "recv " << i << " hash is " << hash << std::endl;
        std::string hash_int;
        boost::algorithm::hex(hash, std::back_inserter(hash_int));
        bm::cpp_int value(hash_int);
        auto xr = e ^ value;
        std::cout << "ans is " << xr << std::endl;
        // for (int i = 0; i < size; ++i)
        // {
        //     bm::cpp_int e{ab[i]};
        //     std::cout << "e is " << e << std::endl;
        //     bm::cpp_int as = pow(a, s, prime);
        //     std::cout << "a is " << e << " b is " << as << std::endl;
        //     auto hash = sha256(as.str() + std::to_string(i));
        //     // std::cout << "recv " << i << " hash is " << hash << std::endl;
        //     std::string hash_int;
        //     boost::algorithm::hex(hash, std::back_inserter(hash_int));
        //     bm::cpp_int value(hash_int);
        //     auto xr = e ^ value;
        //     std::cout << i << ": ans is " << xr << std::endl;
        // }
        return xr;
    }
};
}  // namespace qmpc