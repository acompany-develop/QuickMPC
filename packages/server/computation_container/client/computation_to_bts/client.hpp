#pragma once

#include <string>
#include <vector>
#include "client/helper/helper.hpp"
#include "share/address_id.hpp"
#include "config_parse/config_parse.hpp"
#include "external/proto/engine_to_bts/engine_to_bts.grpc.pb.h"
#include "logging/logger.hpp"

namespace qmpc::ComputationToBts
{
class Client
{
private:
    std::unique_ptr<enginetobts::EngineToBts::Stub> stub_;
    thread_local static inline std::int64_t request_id_generator = 0;

    Client(const Client &) noexcept = delete;
    Client &operator=(const Client &) noexcept = delete;
    Client(Client &&) noexcept = delete;
    Client &operator=(Client &&) noexcept = delete;

    template<typename T>
    enginetobts::GetRequest makeRequest(const unsigned int amount)
    {
        enginetobts::GetRequest request;
        request.set_job_id(qmpc::Share::AddressId::getThreadJobId());
        request.set_amount(amount);
        request.set_request_id(request_id_generator++);
        if constexpr (std::is_same_v<T, float>)
        {
            request.set_type(enginetobts::Type::TYPE_FLOAT);
        }
        else
        {
            request.set_type(enginetobts::Type::TYPE_FIXEDPOINT);
        }
        return request;
    }
public:
    Client() noexcept;
    ~Client() noexcept = default;
    static std::shared_ptr<Client> getInstance();

    template<class Job>
    std::vector<typename Job::result_type> readRequest(const unsigned int amount)
    {
        using T = typename Job::value_type;
        auto request = makeRequest<T>(amount);
        typename Job::response_type response;
        grpc::Status status;

        auto retry_manager = RetryManager("BTS", Job::op_name);
        do
        {
            grpc::ClientContext context;
            const std::string token = Config::getInstance()->cc_to_bts_token;
            context.AddMetadata("authorization", "bearer " + token);
            status = Job::request(stub_, context, request, response);
        }while (retry_manager.retry(status));

        return Job::getValue(response);
    }
};
}  // namespace qmpc::ComputationToBts
