#include "Server.hpp"

#include <deque>
#include <iostream>
#include <string>
#include <thread>

#include "config_parse/config_parse.hpp"
#include "job/JobManager.hpp"
#include "Logging/Logger.hpp"
#include "Server/ComputationToComputationContainerForJob/Server.hpp"
#include "Share/Share.hpp"
#include "external/proto/common_types/common_types.pb.h"
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
    QMPC_LOG_INFO("{:<30} Received", "[ExecuteComputation]");
    Config *conf = Config::getInstance();

    // SPだけ処理
    if (conf->party_id == conf->sp_id)
    {
        auto job_manager = qmpc::Job::JobManager::getInstance();
        job_manager->pushJobReq(*request);

        QMPC_LOG_INFO("{:<30} End grpc", "[ExecuteComputation]");
        return grpc::Status::OK;
    }
    else
    {
        // 別なら即ok返し
        return grpc::Status::OK;
    }
}

grpc::Status Server::CheckProgress(
    grpc::ServerContext *context,
    const managetocomputation::CheckProgressRequest *request,
    pb_common_types::JobProgress *response
)
{
    QMPC_LOG_INFO("{:<30} Received", "[CheckProgress]");
    auto manager = qmpc::Job::ProgressManager::getInstance();

    const std::string &job_uuid = request->job_uuid();

    const auto [progress, code] = manager->getProgress(job_uuid);

    switch (code)
    {
        case qmpc::Job::ProgressManager::StatusCode::NOT_FOUND:
            return grpc::Status(
                grpc::StatusCode::NOT_FOUND,
                (boost::format("progress of job_uuid: %1% was not found") % job_uuid).str()
            );
        case qmpc::Job::ProgressManager::StatusCode::INTERNAL_ERROR:
            return grpc::Status(
                grpc::StatusCode::INTERNAL,
                (boost::format("in processing with job_uuid: %1%, internal error was occured")
                 % job_uuid)
                    .str()
            );
        case qmpc::Job::ProgressManager::StatusCode::OK:
            *response = progress.value();
            return grpc::Status::OK;
    }

    // this line should never be reached
    return grpc::Status(
        grpc::StatusCode::INTERNAL,
        (boost::format("in processing with job_uuid: %1%, something went wrong") % job_uuid).str()
    );
}

void runServer(std::string endpoint)
{
    Server server;

    grpc::ServerBuilder builder;

    builder.RegisterService(&server);

    runServerCore(builder, "Mc2Cc", endpoint);
}
}  // namespace qmpc::ManageToComputation
