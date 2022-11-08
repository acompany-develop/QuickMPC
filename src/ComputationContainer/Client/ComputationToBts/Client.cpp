#include "Client.hpp"

#include <grpc/grpc.h>
#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>

#include <chrono>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>

#include "Client/Helper/Helper.hpp"
#include "ConfigParse/ConfigParse.hpp"
#include "LogHeader/Logger.hpp"
#include "Logging/Logger.hpp"

namespace qmpc::ComputationToBts
{
Client::Client() noexcept
{
    auto dest = Config::getInstance()->cc_to_bts;
    stub_ = createStub<enginetobts::EngineToBts>(dest);
    QMPC_LOG_INFO("{:<15} Client {:<30} is Active", "[Cc2Bts]", dest.url);
}

std::shared_ptr<Client> Client::getInstance()
{
    static auto instance = std::make_shared<Client>();
    return instance;
}

std::vector<Triple> Client::readTriples(const unsigned int job_id, const unsigned int amount)
{
    enginetobts::GetTriplesRequest request;
    request.set_job_id(job_id);
    request.set_amount(amount);
    enginetobts::GetTriplesResponse response;

    grpc::Status status;

    while (true)
    {
        grpc::ClientContext context;

        const std::string token = Config::getInstance()->cc_to_bts_token;
        context.AddMetadata("authorization", "bearer " + token);

        status = stub_->GetTriples(&context, request, &response);
        if (status.ok())
        {
            std::vector<Triple> triples;
            triples.reserve(response.triples_size());
            for (int i = 0; i < response.triples_size(); i++)
            {
                auto triple = response.triples(i);
                Triple t = std::make_tuple(
                    std::to_string(triple.a()),
                    std::to_string(triple.b()),
                    std::to_string(triple.c())
                );
                triples.emplace_back(t);
            }
            return triples;
        }
        else
        {
            grpc::StatusCode error_code = status.error_code();
            QMPC_LOG_ERROR("{:<30} GetFeature rpc failed.", "[readTriples]");
            QMPC_LOG_ERROR(
                "ERROR({}): {}\n{}", error_code, status.error_message(), status.error_details()
            );

            if (error_code == grpc::StatusCode::DEADLINE_EXCEEDED
                || error_code == grpc::StatusCode::UNAVAILABLE
                || error_code == grpc::StatusCode::RESOURCE_EXHAUSTED)
            {
                continue;
            }
            else
            {
                break;
            }
        }
        std::this_thread::sleep_for(std::chrono::seconds(10));
    }

    /*
    ここに到達するのは以下の2パターン
    1. statusがretryPolicyに即した場合に再試行し切ってもOKにならなかった時
    2. retryPolicyに則さずにOK以外の異常なstatusが返ってきた時
    */
    QMPC_LOG_ERROR(
        "To Bts GetTriples Failed, Error Code: {}, Message: {}",
        status.error_code(),
        status.error_message()
    );

    qmpc::Log::throw_with_trace(std::runtime_error("リカバリー不能なエラーが発生"));
}
}  // namespace qmpc::ComputationToBts
