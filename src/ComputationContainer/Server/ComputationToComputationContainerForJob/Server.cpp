#include "Server.hpp"

#include <string>
#include <tuple>

#include "ConfigParse/ConfigParse.hpp"
#include "Job/JobParameter.hpp"
#include "Share/AddressId.hpp"
#include "Job/JobManager.hpp"

#include "LogHeader/Logger.hpp"

namespace qmpc::ComputationToComputationForJob
{
Server::Server() noexcept {}

/*
    ReturnするステータスはJob内でエラーが発生した場合はそれに合わせて変更する
    通常はOK
    status :
        Job外エラー : 0
        Send : 1
        Compute : 2
        Write : 3
        正常完了 : 4
    SPのccから
    計算の開始をSPから受け取る
    TODO
   中身はMC2CCのexecuteComputeと一部同じなので，なんかきれいなアーキテクチャにしたい
*/
grpc::Status Server::ExecuteComputeFromSP(
    grpc::ServerContext *context,
    const computationtocomputationforjob::ExecuteComputeFromSPRequest *request,
    google::protobuf::Empty *response
)
{
    spdlog::info("{:<30} Received", "[ExecuteComputeFromSP]");

    auto job_manager = qmpc::Job::JobManager::getInstance();
    bool is_job_trigger_party = request->is_job_trigger_party();

    // Paramのセッティング
    managetocomputation::ExecuteComputationRequest mc2cc_exec_comp_request(request->exec_request());
    unsigned int job_id = request->job_id();
    qmpc::Job::JobParameter job_param(mc2cc_exec_comp_request, job_id);

    // Jobを走らせる
    job_manager->asyncRun(job_param, is_job_trigger_party);

    spdlog::info("{:<30} End grpc", "[ExecuteComputeFromSP]");
    return grpc::Status::OK;
}

void Server::runServer(std::string endpoint)
{
    auto server = Server::getServer();
    grpc::ServerBuilder builder;
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
    builder.RegisterService(server);
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
            "[Cc2CcForJob]",
            endpoint
        );
    }

    spdlog::info("{:<15} Server listening on {:<30}", "[Cc2CcForJob]", endpoint);
    listener->Wait();
}
}  // namespace qmpc::ComputationToComputationForJob
