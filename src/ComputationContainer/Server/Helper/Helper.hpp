#pragma once

#include <grpc/grpc.h>
#include <grpcpp/security/server_credentials.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>

#include "LogHeader/Logger.hpp"

class LoggingServerInterceptor : public grpc::experimental::Interceptor
{
    std::string grpc_method_full_name;
    std::string server_name;

public:
    explicit LoggingServerInterceptor(
        grpc::experimental::ServerRpcInfo* info, const std::string& server_name
    )
        : grpc_method_full_name(info->method()), server_name(server_name)
    {
    }

    void Intercept(grpc::experimental::InterceptorBatchMethods* methods) override
    {
        if (methods->QueryInterceptionHookPoint(
                grpc::experimental::InterceptionHookPoints::POST_RECV_INITIAL_METADATA
            ))
        {
            // if this was hooked `POST_RECV_MESSAGE`
            // and ServerRpcInfo::Type was `CLIENT_STREAMING`,
            // this was going to be called multiple.
            QMPC_LOG_INFO("{} - [server:{}] received", grpc_method_full_name, server_name);
        }

        if (methods->QueryInterceptionHookPoint(
                grpc::experimental::InterceptionHookPoints::PRE_SEND_STATUS
            ))
        {
            const grpc::Status status = methods->GetSendStatus();

            if (status.ok())
            {
                QMPC_LOG_INFO(
                    "{} - [server:{}] send, gRPC status: {}",
                    grpc_method_full_name,
                    server_name,
                    status.error_code()
                );
            }
            else
            {
                QMPC_LOG_INFO(
                    "{} - [server:{}] send, gRPC status: {}, message: {}, details: {}",
                    grpc_method_full_name,
                    server_name,
                    status.error_code(),
                    status.error_message(),
                    status.error_details()
                );
            }
        }

        methods->Proceed();
    }
};

class LoggingServerInterceptorFactory : public grpc::experimental::ServerInterceptorFactoryInterface
{
private:
    std::string server_name;

public:
    LoggingServerInterceptorFactory(const std::string& server_name) : server_name(server_name) {}

    grpc::experimental::Interceptor* CreateServerInterceptor(grpc::experimental::ServerRpcInfo* info
    ) override
    {
        return new LoggingServerInterceptor(info, server_name);
    }
};

/*
 * @param &builder 呼び出し側で宣言したServerBuilder変数
 * @param logSource ログ発行元を示す文字列（例："[Cc2CcForJob]", "[Cc2Cc]"）
 * @param endpoint エンドポイント
 */
inline void runServerCore(
    grpc::ServerBuilder& builder, const std::string& logSource, const std::string& endpoint
)
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

    std::vector<std::unique_ptr<grpc::experimental::ServerInterceptorFactoryInterface>> creators;
    creators.push_back(std::make_unique<LoggingServerInterceptorFactory>(logSource));
    builder.experimental().SetInterceptorCreators(std::move(creators));

    std::unique_ptr<grpc::Server> listener(builder.BuildAndStart());

    // HealthCheckService を有効化する
    grpc::HealthCheckServiceInterface* service = listener->GetHealthCheckService();
    if (service != nullptr)
    {
        service->SetServingStatus(true);  // 登録した全てのサービスで有効化
    }
    else
    {
        QMPC_LOG_INFO(
            "[{:<15 }] grpc::DefaultHealthCheckService "
            "is not enabled on {:<30}",
            logSource,
            endpoint
        );
    }

    QMPC_LOG_INFO("[{:<15}] Server listening on {:<30}", logSource, endpoint);
    listener->Wait();
}
