#include "Server.hpp"

#include <string>
#include <tuple>

#include "ConfigParse/ConfigParse.hpp"
#include "Job/JobManager.hpp"
#include "Job/JobParameter.hpp"
#include "LogHeader/Logger.hpp"
#include "Share/AddressId.hpp"

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
    builder.RegisterService(server);

    runServerCore(builder, "Cc2CcForJob", endpoint);
}
}  // namespace qmpc::ComputationToComputationForJob
