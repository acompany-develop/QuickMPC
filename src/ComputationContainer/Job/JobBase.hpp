#pragma once

#include <atomic>
#include <list>
#include <map>
#include <vector>

#include "Client/ComputationToDbGate/Client.hpp"
#include "Client/ComputationToDbGate/ValueTable.hpp"
#include "JobParameter.hpp"
#include "JobStatus.hpp"
#include "LogHeader/Logger.hpp"
#include "Share/Share.hpp"
#include "external/Proto/ManageToComputationContainer/manage_to_computation.grpc.pb.h"

namespace qmpc::Job
{
class Interface
{
public:
    virtual int run() = 0;
    virtual ~Interface() {}
};

template <typename T>
class JobBase : public Interface
{
    Config *conf;
    const managetocomputation::ExecuteComputationRequest request;
    const unsigned int job_id;

    std::shared_ptr<qmpc::ComputationToDbGate::Client> db_client =
        qmpc::ComputationToDbGate::Client::getInstance();

    StatusManager statusManager;

    auto readDb()
    {
        statusManager.nextStatus();

        // table結合
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

    static auto toString(const std::vector<::Share> &values)
    {
        std::vector<std::string> results;
        results.reserve(values.size());
        for (const auto &value : values)
        {
            results.emplace_back(value.getVal().getStrVal());
        }
        return results;
    }
    static auto toString(const std::vector<std::vector<::Share>> &values)
    {
        std::vector<std::vector<std::string>> results;
        results.reserve(values.size());
        for (const auto &value : values)
        {
            results.emplace_back(toString(value));
        }
        return results;
    }
    static auto toString(const nlohmann::json &values)
    {
        // 既にjsonならば文字列化しなくてもJsonに乗せられるためそのまま返す
        return values;
    }

    template <typename U>
    void writeDb(U &&values)
    {
        statusManager.nextStatus();
        auto results = toString(values);
        db_client->writeComputationResult(request.job_uuid(), results);
        statusManager.nextStatus();
    }

public:
    using Share = qmpc::Share::Share<FixedPoint>;
    JobBase(const JobParameter &request)
        : request(request.getRequest())
        , job_id(request.getJobId())
        , statusManager(request.getRequest().job_uuid())
    {
        qmpc::Share::AddressId::setJobId(job_id);
    }
    virtual ~JobBase() {}
    int run() override
    {
        std::list<int> src;
        for (const auto &it : request.arg().src())
        {
            src.emplace_back(it);
        }
        std::list<int> target;
        for (const auto &it : request.arg().target())
        {
            target.emplace_back(it);
        }
        std::vector<std::list<int>> arg{src, target};

        try
        {
            auto [share_table, schemas] = readDb();
            auto result = static_cast<T *>(this)->compute(share_table, schemas, arg);
            writeDb(result);
        }
        catch (const std::runtime_error &e)
        {
            spdlog::error("{}", static_cast<int>(statusManager.getStatus()));
            qmpc::Log::Error(*boost::get_error_info<qmpc::Log::traced>(e), e.what(), "Job Error");
        }
        catch (const std::exception &e)
        {
            spdlog::error("unexpected Error");
            qmpc::Log::Error(*boost::get_error_info<qmpc::Log::traced>(e), e.what(), "Job Error");
        }
        return static_cast<int>(statusManager.getStatus());
    }
};
}  // namespace qmpc::Job
