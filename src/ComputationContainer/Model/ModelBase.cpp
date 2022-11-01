#include "ModelBase.hpp"

#include "Job/ProgressManager.hpp"
#include "LogHeader/Logger.hpp"
#include "nlohmann/json.hpp"

namespace qmpc::Model
{
std::pair<std::vector<std::vector<Share>>, std::vector<std::string>> ModelBase::readDb(
    const managetocomputation::PredictRequest &request
)
{
    statusManager.nextStatus();

    // table結合
    auto db_client = qmpc::ComputationToDb::Client::getInstance();
    auto joinTable = db_client->readTable(request.table());
    auto values = joinTable.getTable();

    // シェア型に変換
    int h = values.size();
    int w = values[0].size();
    std::vector<std::vector<Share>> share_table(h, std::vector<Share>(w));
    for (int i = 0; i < h; ++i)
    {
        for (int j = 0; j < w; ++j)
        {
            share_table[i][j] = values[i][j];
        }
    }

    statusManager.nextStatus();
    auto schemas = joinTable.getSchemas();
    return std::pair<std::vector<std::vector<Share>>, std::vector<std::string>>(
        share_table, schemas
    );
}

int ModelBase::run(const managetocomputation::PredictRequest &request)
{
    constexpr int JOB_ID = 1000;  // JobIDのCompetitionを防ぐためJob上限よりも高い値を設定
    qmpc::Share::AddressId::setJobId(JOB_ID);

    qmpc::Job::ProgressManager::getInstance()->registerJob(JOB_ID, request.job_uuid());

    statusManager.initJobID(request.job_uuid());

    std::list<int> src;
    for (const auto &it : request.src())
    {
        src.emplace_back(it);
    }

    std::string dump_result = "";
    try
    {
        auto [share_table, schemas] = readDb(request);
        auto result = predict(share_table, schemas, src, request.model_param_job_uuid());
        writeDb(request, result);
    }
    catch (const std::runtime_error &e)
    {
        spdlog::error("{}", static_cast<int>(statusManager.getStatus()));
        spdlog::error(e.what());
        spdlog::error("Predict Error");
    }
    catch (const std::exception &e)
    {
        spdlog::error("unexpected Error");
        spdlog::error(e.what());
        spdlog::error("Predict Error");
    }

    return static_cast<int>(statusManager.getStatus());
}
}  // namespace qmpc::Model
