#pragma once

#include <string>
#include <vector>

#include "external/Proto/EngineToBts/engine_to_bts.grpc.pb.h"

namespace qmpc::ComputationToBts
{
using Triple = std::tuple<std::string, std::string, std::string>;

class Client
{
private:
    std::unique_ptr<enginetobts::EngineToBts::Stub> stub_;

    Client(const Client &) noexcept = delete;
    Client &operator=(const Client &) noexcept = delete;
    Client(Client &&) noexcept = delete;
    Client &operator=(Client &&) noexcept = delete;

public:
    Client() noexcept;
    ~Client() noexcept = default;
    static std::shared_ptr<Client> getInstance();

    // tripleの取り出し
    std::vector<Triple> readTriples(const unsigned int job_id, const unsigned int amount);
};
}  // namespace qmpc::ComputationToBts
