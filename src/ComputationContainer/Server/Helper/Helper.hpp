#pragma once

#include <grpc/grpc.h>
#include <grpcpp/security/server_credentials.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>

#include "LogHeader/Logger.hpp"

/*
 * @param &builder 呼び出し側で宣言したServerBuilder変数
 * @param logSource ログ発行元を示す文字列（例："[Cc2CcForJob]", "[Cc2Cc]"）
 * @param endpoint エンドポイント
 */
static void runServerCore(grpc::ServerBuilder &builder, std::string logSource, std::string endpoint)
{
    /*
    grpc Server keepalive設定
    GRPC_ARG_HTTP2_MIN_RECV_PING_INTERVAL_WITHOUT_DATA_MS　pingの間隔がこの時間より短い場合は攻撃とみなす　9秒
    （この時間はクライアントと調整する）
    GRPC_ARG_KEEPALIVE_PERMIT_WITHOUT_CALLS　通信がなくてもpingを送る　on
    */
    builder.AddChannelArgument(GRPC_ARG_HTTP2_MIN_RECV_PING_INTERVAL_WITHOUT_DATA_MS, 9000);
    builder.AddChannelArgument(GRPC_ARG_KEEPALIVE_PERMIT_WITHOUT_CALLS, 1);

    // 外部からのSSL通信はインフラレイヤーでhttpに変換するのでInsecureにする
    builder.AddListeningPort(endpoint, grpc::InsecureServerCredentials());

    std::unique_ptr<grpc::Server> listener(builder.BuildAndStart());

    // HealthCheckService を有効化する
    grpc::HealthCheckServiceInterface *service = listener->GetHealthCheckService();
    if (service != nullptr)
    {
        service->SetServingStatus(true);  // 登録した全てのサービスで有効化
    }
    else
    {
        spdlog::info(
            "{:<15 } grpc::DefaultHealthCheckService "
            "is not enabled on {:<30}",
            logSource,
            endpoint
        );
    }

    spdlog::info("{:<15} Server listening on {:<30}", logSource, endpoint);
    listener->Wait();
}
