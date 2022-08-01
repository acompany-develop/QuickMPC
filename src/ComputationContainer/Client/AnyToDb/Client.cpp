#include "Client.hpp"

#include <chrono>

#include <grpc/grpc.h>
#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>

#include "ConfigParse/ConfigParse.hpp"
#include <stdexcept>

#include "LogHeader/Logger.hpp"
#include "Logging/Logger.hpp"
namespace AnyToDb
{

bool Client::reconnect() const
{
    // 再接続に6回(3秒)失敗したら異常終了
    const uint16_t RECONNECT_LIMIT = 6;
    uint16_t cnt = 0;
    while (true)
    {
        // 0.5秒ごとに再接続を試みる
        spdlog::info("reconnecting to {} ...", host);
        if (channel->WaitForConnected(
                std::chrono::system_clock::now() + std::chrono::milliseconds(500)
            ))
            break;
        cnt++;

        if (cnt >= RECONNECT_LIMIT)
            qmpc::Log::throw_with_trace(std::runtime_error("connect DB time out !"));
    }
    spdlog::info("connect success!");
    return true;
}

std::shared_ptr<grpc::Channel> Client::connect()
{
    auto dest = Config::getInstance()->any_to_dbg;
    std::string server_address(dest.host + ':' + dest.port);

    auto channel = grpc::CreateChannel(server_address, grpc::InsecureChannelCredentials());
    return channel;
}

Client::Client(const std::string &host)
    : host(host), channel(Client::connect()), stub_(anytodbgate::AnyToDbGate::NewStub(channel))
{
}

std::string Client::executeQuery(const std::string &query) const
{
    // request setting
    auto request = anytodbgate::ExecuteQueryRequest();
    request.set_host(host);
    request.set_query(query);

    // response setting
    auto context = grpc::ClientContext();
    auto response = anytodbgate::ExecuteQueryResponse();

    // クエリ送信
    std::unique_ptr<grpc::ClientReader<anytodbgate::ExecuteQueryResponse>> stream(
        stub_->ExecuteQuery(&context, request)
    );

    std::map<int, std::string> pieces;

    while (stream->Read(&response))
    {
        int piece_id = response.piece_id();
        std::string result = response.result();
        pieces.emplace(piece_id, result);
    }

    grpc::Status status = stream->Finish();
    if (status.ok())
    {
        std::string res;
        for (const auto &[_, piece] : pieces)
        {
            res.append(piece);
        }
        return res;
    }

    // エラー発生時
    spdlog::error(
        "To DbGate Execute Query Failed, Error Code: {}, Message:{}",
        status.error_code(),
        status.error_message()
    );
    if (status.error_code() == grpc::StatusCode::UNAVAILABLE)
    {
        reconnect();
        return executeQuery(query);
    }
    qmpc::Log::throw_with_trace(std::runtime_error("DBClientError : To DbGate Execute Query Failed")
    );
}

}  // namespace AnyToDb
