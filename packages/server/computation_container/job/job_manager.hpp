#pragma once

#include <future>
#include <memory>
#include <thread>
#include <unordered_map>
#include <vector>

#include "client/computation_to_bts/client.hpp"
#include "client/computation_to_computation_container_for_job/client.hpp"
#include "config_parse/config_parse.hpp"
#include "job_base.hpp"
#include "job_parameter.hpp"
#include "job_selector.hpp"
#include "job_status.hpp"
#include "logging/logger.hpp"
#include "progress_manager.hpp"
#include "transaction_queue/transaction_queue.hpp"

namespace qmpc::Job
{
/*
Jobを管理するクラス
runJobManagerでJobReqをQueueから出してJobの情報を全Partyに伝える
    job_req_queueで処理予定のJobを管理
    job_id_queueで使用可能なJobIdを管理
*/
class JobManager
{
    inline static constexpr unsigned int MAX_JOB_NUM = 5;
    JobSelector selector;
    std::map<int, std::shared_ptr<qmpc::ComputationToComputationForJob::Client>> cc_for_job_clients;
    qmpc::Utils::TransactionQueue<unsigned int> job_id_queue;
    qmpc::Utils::TransactionQueue<managetocomputation::ExecuteComputationRequest> job_req_queue;
    // TODO:AddressIDを管理するクラスもここに配置する

    // JobIDをjob_id_queueに入れる
    void pushJobId(const unsigned int job_id) { job_id_queue.push(job_id); }

    // JobIDをjob_id_queueから出す
    unsigned int pollingPopJobId() { return job_id_queue.pop(); }

    // try-catchをしながら特定の処理を実行する
    template <class F>
    static auto try_catch_run(const std::string &job_uuid, const F &func)
    {
        StatusManager statusManager(job_uuid);
        auto exception_post_process = [&](const std::exception &e)
        {
            QMPC_LOG_ERROR("error job_uuid is {}", job_uuid);
            QMPC_LOG_ERROR("{} | Job Error", e.what());

            auto error_info = boost::get_error_info<qmpc::Log::traced>(e);
            if (error_info)
            {
                QMPC_LOG_ERROR("{}", *error_info);
                statusManager.error(e, *error_info);
            }
            else
            {
                QMPC_LOG_ERROR("thrown exception has no stack trace information");
                statusManager.error(e, std::nullopt);
            }
            ProgressManager::getInstance()->unregisterJob(job_uuid);
        };

        try
        {
            func();
            return true;
        }
        catch (const std::runtime_error &e)
        {
            QMPC_LOG_ERROR("{}", static_cast<int>(statusManager.getStatus()));
            exception_post_process(e);
        }
        catch (const std::exception &e)
        {
            QMPC_LOG_ERROR("unexpected Error");
            exception_post_process(e);
        }
        return false;
    }

public:
    JobManager()
    {
        for (unsigned int job_id = 1; job_id <= MAX_JOB_NUM; ++job_id)
        {
            pushJobId(job_id);
        }

        Config *conf = Config::getInstance();
        for (int pt_id = 1; pt_id <= conf->n_parties; ++pt_id)
        {
            auto client = qmpc::ComputationToComputationForJob::Client::getPtr(
                conf->ip_addr_for_job_map[pt_id]
            );
            cc_for_job_clients[pt_id] = std::move(client);
            QMPC_LOG_INFO("{:<15} Client {:<30} is Active", "[Cc2CcForJob]", pt_id);
        }
    };

    static std::shared_ptr<JobManager> getInstance()
    {
        static auto instance = std::make_shared<JobManager>();
        return instance;
    }

    std::shared_ptr<qmpc::ComputationToComputationForJob::Client> getCcForJobClient(const int pt_id)
    {
        return cc_for_job_clients[pt_id];
    }

    // JobReqをjob_req_queueに入れる
    void pushJobReq(const managetocomputation::ExecuteComputationRequest &job_param)
    {
        job_req_queue.push(job_param);
    }

    // JobReqをjob_req_queueから出す
    // NOTE popの返り値は参照にしたほうがいいかも
    managetocomputation::ExecuteComputationRequest pollingPopJobReq()
    {
        return job_req_queue.pop();
    }

    // jobを走らせる
    void asyncRun(const JobParameter &job_param, const bool is_job_trigger_party)
    {
        // Job実行の中身
        auto job_id = job_param.getJobId();
        auto job_uuid = job_param.getRequest().job_uuid();
        QMPC_LOG_INFO("start job_id is {}", job_id);
        QMPC_LOG_INFO("start job_uuid is {}", job_uuid);
        QMPC_LOG_INFO("JobManager: method Id is {}", job_param.getRequest().method_id());

        ProgressManager::getInstance()->registerJob(job_id, job_uuid);

        std::thread job_thread(
            [=]
            {
                auto job = this->selector(job_param);
                if (job == nullptr)
                {
                    QMPC_LOG_ERROR("unknown Method Id");
                    QMPC_LOG_ERROR("Request Failed");
                    // TODO: statusをERRORにする
                }
                else
                {
                    try_catch_run(job_uuid, [&]() { job->run(); });
                }
                QMPC_LOG_INFO("end job_id is {}", job_id);
                QMPC_LOG_INFO("end job_uuid is {}", job_uuid);
                if (is_job_trigger_party) pushJobId(job_id);
            }
        );
        job_thread.detach();  // TODO Detachはスレッド管理者が消えるので，アカンかも．．．
    }

    // 全Party（自分も含む）にJobの情報を送り，Jobを開始させる
    void sendJobInfo(
        const managetocomputation::ExecuteComputationRequest &job_req, const unsigned int &job_id
    )
    {
        Config *conf = Config::getInstance();
        for (int pt_id = 1; pt_id <= conf->n_parties; ++pt_id)
        {
            bool is_job_trigger_party = (pt_id == conf->party_id);
            this->getCcForJobClient(pt_id)->executeComputeFromSP(
                job_req, job_id, is_job_trigger_party
            );
        }
    }

    /*
     Jobの実行を行わせるための関数
     1. JobReqQueueに何かが入ってくるまで待機してそれを取ってくる
     2. JobIDを決める
     3. JobReqの内容を全パーティに伝える
    */
    static void runJobManager()
    {
        auto job_manager = getInstance();
        QMPC_LOG_INFO("Job Manager Start");
        while (true)
        {
            // 1. JobReqQueueに何かが入ってくるまで待機してそれを取ってくる
            auto job_req = job_manager->pollingPopJobReq();

            // 2. JobIDを決める
            auto job_id = job_manager->pollingPopJobId();

            // 3. 取りだしたJobReqの内容を全パーティに伝える
            try_catch_run(job_req.job_uuid(), [&]() { job_manager->sendJobInfo(job_req, job_id); });
        }
    }
};
}  // namespace qmpc::Job
