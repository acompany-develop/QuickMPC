#include "Computation.hpp"

#include "Logging/Logger.hpp"

bool is_computing = false;

bool compute(
    std::vector<std::string> &values,
    std::vector<std::string> &data_ids,
    __int32_t methodId,
    std::vector<std::string> &results
)
{
    auto start = std::chrono::system_clock::now();
    is_computing = true;

    std::vector<Share> shares;
    parseData(values, data_ids, shares);

    // mean
    if (methodId == 1)
    {
        Share avg;
        avg = qmpc::Math::smean(shares);
        results.push_back(avg.getVal().getStrVal());
    }
    else if (methodId == 2)
    {
        Share var;
        var = qmpc::Math::variance(shares);
        results.push_back(var.getVal().getStrVal());
    }
    else
    {
        return false;
    }

    is_computing = false;

    auto end = std::chrono::system_clock::now();
    auto dur = end - start;

    // 計算に要した時間をミリ秒（1/1000秒）に変換して表示
    auto msec = std::chrono::duration_cast<std::chrono::milliseconds>(dur).count();

    QMPC_LOG_INFO("method ID: {}", methodId);
    QMPC_LOG_INFO("culc time: {} milli sec", msec);
    return true;
}

// 複数のdataをパースし, shareのvectorに連結
void parseData(
    std::vector<std::string> &values, std::vector<std::string> &data_ids, std::vector<Share> &shares
)
{
    for (int i = 0; i < (int)data_ids.size(); i++)
    {
        nlohmann::json json = nlohmann::json::parse(values[i]);
        std::string data_id = data_ids[i];
        int n = json[data_id][0].size();

        for (int j = 0; j < n; j++)
        {
            Share a(FixedPoint(std::stod(json[data_id][0][j].get<std::string>())));
            QMPC_LOG_DEBUG("Share: {}", a.getVal().getStrVal());
            shares.emplace_back(a);
        }
    }
}