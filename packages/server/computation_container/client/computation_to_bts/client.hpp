#pragma once

#include <string>
#include <vector>

#include "client/helper/helper.hpp"
#include "config_parse/config_parse.hpp"
#include "external/proto/engine_to_bts/engine_to_bts.grpc.pb.h"
#include "logging/logger.hpp"
#include "share/address_id.hpp"

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

    enginetobts::GetRequest makeRequest(const unsigned int amount)
    {
        enginetobts::GetRequest request;
        request.set_job_id(qmpc::Share::AddressId::getThreadJobId());
        request.set_amount(amount);
        request.set_request_id(request_id_generator++);
        return request;
    }

public:
    Client() noexcept;
    ~Client() noexcept = default;
    static std::shared_ptr<Client> getInstance();

    template <class BTSJob>
    std::vector<typename BTSJob::result_type> readRequest(const unsigned int amount)
    {
        auto request = makeRequest(amount);
        typename BTSJob::response_type response;
        grpc::Status status;

        auto retry_manager = RetryManager("BTS", BTSJob::op_name);
        do
        {
            grpc::ClientContext context;
            const std::string token = Config::getInstance()->cc_to_bts_token;
            context.AddMetadata("authorization", "bearer " + token);
            status = BTSJob::request(stub_, context, request, response);
        } while (retry_manager.retry(status));

        return BTSJob::getValue(response);
    }
};
}  // namespace qmpc::ComputationToBts
