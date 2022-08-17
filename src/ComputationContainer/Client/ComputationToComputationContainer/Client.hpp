#pragma once

#include <grpc/grpc.h>
#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>

#include <iostream>
#include <string>

#include "Client/Helper/Helper.hpp"
#include "ConfigParse/ConfigParse.hpp"
#include "Share/AddressId.hpp"
#include "external/Proto/ComputationToComputationContainer/computation_to_computation.grpc.pb.h"

namespace qmpc::ComputationToComputation
{
class Client
{
    friend class Server;

private:
    std::unique_ptr<computationtocomputation::ComputationToComputation::Stub> stub_;
    Client() noexcept = default;
    ~Client() noexcept = default;
    Client(const Url &endpoint) noexcept;
    Client &operator=(Client &&) noexcept = default;
    static std::shared_ptr<Client> getPtr(const Url &);

public:
    Client(Client &&) noexcept = delete;
    Client(const Client &) noexcept = delete;
    Client &operator=(const Client &) noexcept = delete;
    bool exchangeShare(std::string value, qmpc::Share::AddressId share_id, int party_id) const;
    bool exchangeShares(
        std::string values[], qmpc::Share::AddressId share_ids[], unsigned int length, int party_id
    ) const;
};

computationtocomputation::Share makeShare(std::string value, int share_id, int party_id);
std::vector<computationtocomputation::Shares> makeShares(
    std::string values[], int share_ids[], unsigned int length, int party_id
);

}  // namespace qmpc::ComputationToComputation
