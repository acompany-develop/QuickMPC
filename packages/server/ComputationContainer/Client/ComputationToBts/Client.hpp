#pragma once

#include <string>
#include <vector>

#include "Client/Helper/Helper.hpp"
#include "ConfigParse/ConfigParse.hpp"
#include "Logging/Logger.hpp"
#include "external/proto/EngineToBts/engine_to_bts.grpc.pb.h"

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
    template <class SV>
    std::vector<Triple> readTriples(const unsigned int job_id, const unsigned int amount)
    {
        // リクエスト設定
        enginetobts::GetTriplesRequest request;
        request.set_job_id(job_id);
        request.set_amount(amount);
        if constexpr (std::is_same_v<SV, float>)
        {
            request.set_triple_type(enginetobts::Type::TYPE_FLOAT);
        }
        else
        {
            request.set_triple_type(enginetobts::Type::TYPE_FIXEDPOINT);
        }
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

    void initTripleStore(){
        grpc::Status status;
        google::protobuf::Empty request, response;

        // リトライポリシーに従ってリクエストを送る
        auto retry_manager = RetryManager("BTS", "initTripleStore");
        do
        {
            grpc::ClientContext context;
            const std::string token = Config::getInstance()->cc_to_bts_token;
            context.AddMetadata("authorization", "bearer " + token);
            status = stub_->InitTripleStore(&context, request, &response);
        } while (retry_manager.retry(status));
    }
};
}  // namespace qmpc::ComputationToBts
