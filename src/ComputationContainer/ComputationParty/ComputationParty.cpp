#include "ComputationParty.hpp"
#include <iostream>
#include <string>
#include <thread>
#include <regex>
#include "unistd.h"

#include <grpcpp/health_check_service_interface.h>

#include "ConfigParse/ConfigParse.hpp"
#include "Server/ComputationToComputationContainer/Server.hpp"
#include "Server/ComputationToComputationContainerForJob/Server.hpp"
#include "Server/ManageToComputationContainer/Server.hpp"
#include "Job/JobManager.hpp"

#include "LogHeader/Logger.hpp"

int main()
{
    Config *conf = Config::getInstance();

    const Url ctoc_ip = conf->ip_addr_map[conf->party_id];
    const Url ctoc_for_j_ip = conf->ip_addr_for_job_map[conf->party_id];
    const Url mtoc_ip = conf->mc_to_cc;

    grpc::EnableDefaultHealthCheckService(true);

    const std::string ctc_my_ip_str("0.0.0.0:" + ctoc_ip.port);
    std::thread th1(qmpc::ComputationToComputation::Server::runServer, ctc_my_ip_str);
    const std::string ctc_for_j_my_ip_str("0.0.0.0:" + ctoc_for_j_ip.port);
    std::thread th2(qmpc::ComputationToComputationForJob::Server::runServer, ctc_for_j_my_ip_str);
    const std::string mtoc_ip_str("0.0.0.0:" + mtoc_ip.port);
    std::thread th3(qmpc::ManageToComputation::runServer, mtoc_ip_str);
    std::thread th4(qmpc::Job::JobManager::runJobManager);
    spdlog::info(
        "My_ip_addr = [Cc2Cc] {0:<15} | [Cc2CcForJob] {1:<15} |[Mc2Cc] %{2:<15}",
        ctc_my_ip_str,
        ctc_for_j_my_ip_str,
        mtoc_ip_str
    );

    th1.join();
    th2.join();
    th3.join();
    th4.join();
}
