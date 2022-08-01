#include "Client.hpp"
#include "Share/AddressId.hpp"
#include <string>
#include <unistd.h>
#include <thread>
#include <chrono>

#include "LogHeader/Logger.hpp"
namespace qmpc::ComputationToComputationForJob
{
Client::Client(const Url &endpoint) noexcept
{
    stub_ = createStub<computationtocomputationforjob::ComputationToComputationForJob>(endpoint);
    spdlog::info("{:<15} Client {:<30} is Active", "[Cc2CcForJob]", endpoint.url);
}

std::shared_ptr<Client> Client::getPtr(const Url &endpoint)
{
    /*
    shared_ptrでprivate constructorを管理するために作成したクラス
    外部からは作成できないため一時的にpublic constructorを作成できる
    スコープを抜けると消滅するため安全
    返却する型は元のクラスになるのでupCastされてオブジェクトはClientになる
    */
    struct impl : Client
    {
        impl(const Url &url) : Client(url) {}
    };
    auto ptr = std::make_shared<impl>(endpoint);
    return std::move(ptr);
}

// Job実行を起動させるCC2CCサービスのClient
bool Client::executeComputeFromSP(
    const managetocomputation::ExecuteComputationRequest &mc2cc_exec_comp_request,
    const unsigned int job_id,
    const bool is_job_trigger_party
) const
{
    computationtocomputationforjob::ExecuteComputeFromSPRequest execute_compute_from_sp_request;
    google::protobuf::Empty response;
    execute_compute_from_sp_request = convertExecuteComputationRequest(mc2cc_exec_comp_request);
    execute_compute_from_sp_request.set_job_id(job_id);
    execute_compute_from_sp_request.set_is_job_trigger_party(is_job_trigger_party);

    while (true)
    {
        grpc::ClientContext context;
        grpc::Status status =
            stub_->ExecuteComputeFromSP(&context, execute_compute_from_sp_request, &response);
        // spdlog::info("exchange message: share_id is {}, party_id is {}, value is
        // {}",share_id,party_id,share_id);

        if (!status.ok())
        {
            spdlog::error(
                "{:<30} GetFeature rpc failed. or job not finished yet.", "[executeComputeFromSP]"
            );
            spdlog::error(
                "ERROR({0}): {1}\n{2}",
                status.error_code(),
                status.error_message(),
                status.error_details()
            );
        }
        else
            return true;
        std::this_thread::sleep_for(std::chrono::seconds(10));
    }
}

// MC2CCのExecuteComputationRequestをCC2CCのexecuteComputeFromSPRequestに変換するやつ
// 中身をひたすらコピーしてるだけ
computationtocomputationforjob::ExecuteComputeFromSPRequest convertExecuteComputationRequest(
    const managetocomputation::ExecuteComputationRequest &mc2cc_exec_comp_request
)
{
    computationtocomputationforjob::ExecuteComputeFromSPRequest execute_compute_from_sp_request;

    auto mc_request = execute_compute_from_sp_request.mutable_exec_request();
    mc_request->set_method_id(mc2cc_exec_comp_request.method_id());
    mc_request->set_job_uuid(mc2cc_exec_comp_request.job_uuid());

    auto mc_request_table = mc_request->mutable_table();
    for (auto &data_id : mc2cc_exec_comp_request.table().dataids())
    {
        mc_request_table->add_dataids(data_id);
    }
    for (auto &join : mc2cc_exec_comp_request.table().join())
    {
        mc_request_table->add_join(join);
    }
    for (auto &index : mc2cc_exec_comp_request.table().index())
    {
        mc_request_table->add_index(index);
    }

    auto mc_request_arg = mc_request->mutable_arg();
    for (auto &src : mc2cc_exec_comp_request.arg().src())
    {
        mc_request_arg->add_src(src);
    }
    for (auto &target : mc2cc_exec_comp_request.arg().target())
    {
        mc_request_arg->add_target(target);
    }

    return execute_compute_from_sp_request;
}

}  // namespace qmpc::ComputationToComputationForJob
