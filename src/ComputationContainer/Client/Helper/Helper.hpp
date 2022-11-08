#pragma once

#include <grpc/grpc.h>
#include <grpcpp/channel.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>

#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "ConfigParse/ConfigParse.hpp"

class LoggingClientInterceptor : public grpc::experimental::Interceptor
{
    std::string grpc_method_full_name;

    bool send_messsage_status;
    std::string request;
    std::string response;

public:
    explicit LoggingClientInterceptor(grpc::experimental::ClientRpcInfo *info)
        : grpc_method_full_name(info->method())
    {
    }

    void Intercept(grpc::experimental::InterceptorBatchMethods *methods) override
    {
        if (methods->QueryInterceptionHookPoint(
                grpc::experimental::InterceptionHookPoints::PRE_SEND_MESSAGE
            ))
        {
            QMPC_LOG_INFO("{} - [client] send", grpc_method_full_name);
        }

        if (methods->QueryInterceptionHookPoint(
                grpc::experimental::InterceptionHookPoints::POST_RECV_STATUS
            ))
        {
            const grpc::Status *status = methods->GetRecvStatus();

            if (status == nullptr)
            {
                QMPC_LOG_WARN("{} - [client] received, gRPC status: nullptr", grpc_method_full_name);
            }
            else
            {
                if (status->ok())
                {
                    QMPC_LOG_INFO(
                        "{} - [client] received, gRPC status: {}",
                        grpc_method_full_name,
                        status->error_code()
                    );
                }
                else
                {
                    QMPC_LOG_INFO(
                        "{} - [client] received, gRPC status: {}, message: {}, details: {}",
                        grpc_method_full_name,
                        status->error_code(),
                        status->error_message(),
                        status->error_details()
                    );
                }
            }
        }

        methods->Proceed();
    }
};

class LoggingClientInterceptorFactory : public grpc::experimental::ClientInterceptorFactoryInterface
{
public:
    grpc::experimental::Interceptor *CreateClientInterceptor(grpc::experimental::ClientRpcInfo *info
    ) override
    {
        return new LoggingClientInterceptor(info);
    }
};

/**
 * gRPC チャンネル生成時に渡すデフォルト設定を返す
 */
static grpc::ChannelArguments getDefaultChannelArguments(
    const int keepalive_time_ms = 10000,
    const int keepalive_timeout_ms = 10000,
    const bool keepalive_permit_without_calls = true
)
{
    grpc::ChannelArguments args;
    /*
      grpcClient　keepalive設定
      GRPC_ARG_KEEPALIVE_TIME_MS　pingの送信間隔　10秒
      GRPC_ARG_KEEPALIVE_TIMEOUT_MS　pingの返信待ち時間　10秒
      GRPC_ARG_KEEPALIVE_PERMIT_WITHOUT_CALLS 通信がなくてもpingだけ送る　on
    */
    args.SetInt(GRPC_ARG_KEEPALIVE_TIME_MS, keepalive_time_ms);
    args.SetInt(GRPC_ARG_KEEPALIVE_TIMEOUT_MS, keepalive_timeout_ms);
    args.SetInt(GRPC_ARG_KEEPALIVE_PERMIT_WITHOUT_CALLS, keepalive_permit_without_calls ? 1 : 0);

    return args;
}

/**
 * gRPC の stub を生成して返す．
 * args で渡されたチャンネル設定を使用し，target と通信するための channel を生成する．
 */
template <typename T>
static std::unique_ptr<typename T::Stub> createStub(
    const Url &target, const grpc::ChannelArguments &args = getDefaultChannelArguments()
)
{
    // プロトコルに合わせて credentials を設定
    std::shared_ptr<grpc::ChannelCredentials> creds;
    if (target.protocol == "http")
    {
        creds = grpc::InsecureChannelCredentials();
    }
    else if (target.protocol == "https")
    {
        creds = grpc::SslCredentials(grpc::SslCredentialsOptions());
    }
    std::vector<std::unique_ptr<grpc::experimental::ClientInterceptorFactoryInterface>> creators;
    creators.push_back(std::make_unique<LoggingClientInterceptorFactory>());

    auto channel =
        CreateCustomChannelWithInterceptors(target.getAddress(), creds, args, std::move(creators));

    return T::NewStub(channel);
}
