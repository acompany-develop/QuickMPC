#include "computation_benchmark.hpp"

#include "logging/logger.hpp"
#include "external/com_github_grpc_grpc/src/proto/grpc/health/v1/health.grpc.pb.h"
#include "gtest/gtest.h"

// 25分経過するとプログラムを終了させる
void timekeep()
{
    sleep(1500);
    std::terminate();
}

// 通信を含んだテストをmain文で実行しています
int main(int argc, char **argv)
{
    Config *conf = Config::getInstance();

    grpc::EnableDefaultHealthCheckService(true);

    Url ctoc_ip = conf->ip_addr_map[conf->party_id];
    const std::string ctc_my_ip_str("0.0.0.0:" + ctoc_ip.port);
    std::thread server_thread(qmpc::ComputationToComputation::Server::runServer, ctc_my_ip_str);
    QMPC_LOG_INFO("ip_addr: {}", ctc_my_ip_str);

    // healthcheck 用のスレッド
    std::vector<std::thread> healthcheck_threads;
    // スレッドに渡す healthcheck 処理コード
    auto do_healthcheck = [&](const Url &pt_url)
    {
        auto stub = createStub<grpc::health::v1::Health>(pt_url);
        for (;;)
        {
            grpc::ClientContext context;
            grpc::health::v1::HealthCheckRequest request;
            grpc::health::v1::HealthCheckResponse response;
            const grpc::Status status = stub->Check(&context, request, &response);
            if (status.ok() && response.status() == grpc::health::v1::HealthCheckResponse::SERVING)
            {
                break;
            }
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    };
    // healthcheck スレッド立ち上げ
    for (int pt_id = 1; pt_id <= conf->n_parties; ++pt_id)
    {
        if (pt_id != conf->party_id)
        {
            healthcheck_threads.emplace_back(std::thread(do_healthcheck, conf->ip_addr_map[pt_id]));
        }
    }
    // healthcheck スレッド終了待機
    for (auto &th : healthcheck_threads)
    {
        th.join();
    }

    // 時間計測開始
    std::thread timekeeper_thread(timekeep);

    // インテグレーションテストを実行
    ::testing::InitGoogleTest(&argc, argv);
    int ok = RUN_ALL_TESTS();
    if (ok == 0)
        std::exit(EXIT_SUCCESS);
    else
        std::exit(EXIT_FAILURE);

    server_thread.join();
    timekeeper_thread.detach();
}
