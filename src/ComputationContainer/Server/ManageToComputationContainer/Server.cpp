#include "Server.hpp"

#include <deque>
#include <iostream>
#include <string>
#include <thread>

#include "Client/ComputationToDbGate/Client.hpp"
#include "ConfigParse/ConfigParse.hpp"
#include "Job/JobManager.hpp"
#include "LogHeader/Logger.hpp"
#include "Model/ModelManager.hpp"
#include "Server/ComputationToComputationContainerForJob/Server.hpp"
#include "Share/Share.hpp"
#include "unistd.h"

namespace qmpc::ManageToComputation
{
/*
    ReturnするステータスはJob内でエラーが発生した場合はそれに合わせて変更する
    通常はOK
    status :
        Job外エラー : 0
        Send : 1
        Compute : 2
        Write : 3
        正常完了 : 4
*/
grpc::Status Server::ExecuteComputation(
    grpc::ServerContext *context,
    const managetocomputation::ExecuteComputationRequest *request,
    google::protobuf::Empty *response
)
{
    spdlog::info("{:<30} Received", "[ExecuteComputation]");
    Config *conf = Config::getInstance();

    // SPだけ処理
    if (conf->party_id == conf->sp_id)
    {
        auto job_manager = qmpc::Job::JobManager::getInstance();
        job_manager->pushJobReq(*request);

        spdlog::info("{:<30} End grpc", "[ExecuteComputation]");
        return grpc::Status::OK;
    }
    else
    {
        // 別なら即ok返し
        return grpc::Status::OK;
    }
}

/*
    ReturnするステータスはPredict内でエラーが発生した場合はそれに合わせて変更する
    通常はOK
    status :
        Predict外エラー : 0
        Send : 1
        Compute : 2
        正常完了 : 4
*/
grpc::Status Server::Predict(
    grpc::ServerContext *context,
    const managetocomputation::PredictRequest *request,
    google::protobuf::Empty *response
)
{
    spdlog::info("{:<30} Received", "[Predict]");
    auto manager = qmpc::Model::ModelManager();

    auto status = manager.push(*request);

    switch (status)
    {
        case 0:
            spdlog::error("Error predict");
            spdlog::error("Can not Create Predictor");
            return grpc::Status{
                grpc::StatusCode::INVALID_ARGUMENT, grpc::string{"Predict Method Error"}};
        case 1:
            spdlog::error("ReadDB Error");
            return grpc::Status{grpc::StatusCode::INTERNAL, grpc::string{"Read Db Error"}};
        case 2:
            spdlog::error("Predict Error");
            return grpc::Status{grpc::StatusCode::ABORTED, grpc::string{"Computation Error"}};
    }

    spdlog::info("{:<30} End grpd", "[Predict]");
    return grpc::Status::OK;
}

void runServer(std::string endpoint)
{
    Server server;

    grpc::ServerBuilder builder;

    builder.AddListeningPort(endpoint, grpc::InsecureServerCredentials());
    builder.RegisterService(&server);
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
            "{:<15} grpc::DefaultHealthCheckService is not enabled on {:<30}", "[Mc2Cc]", endpoint
        );
    }

    spdlog::info("{:<15} Server listening on {:<30}", "[Mc2Cc]", endpoint);
    listener->Wait();
}
}  // namespace qmpc::ManageToComputation
