#pragma once

#include <memory>
#include <string>
#include <vector>

#include "external/Proto/AnyToDbGate/any_to_db_gate.grpc.pb.h"

namespace AnyToDb
{
class Client
{
    const std::string host;
    const std::shared_ptr<grpc::Channel> channel;
    const std::unique_ptr<anytodbgate::AnyToDbGate::Stub> stub_;
    static std::shared_ptr<grpc::Channel> connect();
    bool reconnect() const;

public:
    Client() = delete;
    Client(const std::string &);

    std::string executeQuery(const std::string &) const;
};
}  // namespace AnyToDb
