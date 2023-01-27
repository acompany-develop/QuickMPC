#include "ModelBase.hpp"
#include <boost/algorithm/string/join.hpp>
#include <boost/range/adaptor/indexed.hpp>
#include <boost/range/adaptor/transformed.hpp>

#include "Logging/Logger.hpp"
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

void ModelBase::validate_cols(const std::vector<std::string> &schemas, const std::list<int> &src)
{
    for (const auto &iter_col : src | boost::adaptors::indexed(0))
    {
        const int col = iter_col.value();
        if (col <= 0 || schemas.size() < static_cast<std::size_t>(col))
        {
            qmpc::Log::throw_with_trace(std::invalid_argument(
                (boost::format("src[%1%] = %3% is out of range where column range = "
                               "[1, %4%], schemas = [%5%]")
                 % iter_col.index() % col % schemas.size()
                 % boost::algorithm::join(
                     schemas
                         | boost::adaptors::transformed(
                             [](const std::string &s)
                             {
                                 std::ostringstream ss;
                                 ss << std::quoted(s);
                                 return ss.str();
                             }
                         ),
                     ", "
                 ))
                    .str()
            ));
        }
    }
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

    auto [share_table, schemas] = readDb(request);
    validate_cols(schemas, src);
    auto result = predict(share_table, schemas, src, request.model_param_job_uuid());
    writeDb(request, result);

    return static_cast<int>(statusManager.getStatus());
}
}  // namespace qmpc::Model
