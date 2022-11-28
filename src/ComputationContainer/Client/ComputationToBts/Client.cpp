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
    // リクエスト設定
    enginetobts::GetTriplesRequest request;
    request.set_job_id(job_id);
    request.set_amount(amount);
    enginetobts::GetTriplesResponse response;

    grpc::Status status;

    // リトライポリシーに従ってリクエストを送る
    auto retry_manager = RetryManager("BTS", "readTriples");
    do
    {
        grpc::ClientContext context;
        const std::string token = Config::getInstance()->cc_to_bts_token;
        context.AddMetadata("authorization", "bearer " + token);
        status = stub_->GetTriples(&context, request, &response);
    } while (retry_manager.retry(status));

    // responseから結果を取り出して返す
    std::vector<Triple> triples;
    triples.reserve(response.triples_size());
    for (int i = 0; i < response.triples_size(); i++)
    {
        auto triple = response.triples(i);
        Triple t = std::make_tuple(
            std::to_string(triple.a()), std::to_string(triple.b()), std::to_string(triple.c())
        );
        triples.emplace_back(t);
    }
    return triples;
}
}  // namespace qmpc::ComputationToBts
