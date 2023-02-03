#pragma once

#include <atomic>
#include <iomanip>
#include <list>
#include <map>
#include <sstream>
#include <stdexcept>
#include <vector>

#include <boost/algorithm/string/join.hpp>
#include <boost/range/adaptor/indexed.hpp>
#include <boost/range/adaptor/transformed.hpp>

#include "client/computation_to_db/client.hpp"
#include "client/computation_to_db/ValueTable.hpp"
#include "JobParameter.hpp"
#include "JobStatus.hpp"
#include "logging/Logger.hpp"
#include "share/share.hpp"
#include "external/proto/manage_to_computation_container/manage_to_computation.grpc.pb.h"

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

    std::shared_ptr<qmpc::ComputationToDb::Client> db_client =
        qmpc::ComputationToDb::Client::getInstance();

    StatusManager statusManager;

    auto readDb()
    {
        statusManager.nextStatus();

        // table結合
        auto joinTable = db_client->readTable(request.table());
        auto values = joinTable.getTable();
        if (values.empty())
        {
            QMPC_LOG_WARN("Join table is empty. Check the table ID and the specified columns.");
        }

        // シェア型に変換
        std::vector<std::vector<Share>> share_table;
        share_table.reserve(values.size());
        for (const auto &rows : values)
        {
            share_table.emplace_back(rows.begin(), rows.end());
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

    static void validate_cols(
        const std::vector<std::string> &schemas, const std::vector<std::list<int>> &arg
    )
    {
        for (const auto &iter_arg : arg | boost::adaptors::indexed(0))
        {
            for (const auto &iter_col : iter_arg.value() | boost::adaptors::indexed(0))
            {
                const int col = iter_col.value();
                if (col <= 0 || schemas.size() < static_cast<std::size_t>(col))
                {
                    qmpc::Log::throw_with_trace(std::invalid_argument(
                        (boost::format("inp[%1%][%2%] = %3% is out of range where column range = "
                                       "[1, %4%], schemas = [%5%]")
                         % iter_arg.index() % iter_col.index() % col % schemas.size()
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

        auto [share_table, schemas] = readDb();
        validate_cols(schemas, arg);
        auto result = static_cast<T *>(this)->compute(share_table, schemas, arg);
        writeDb(result);

        return static_cast<int>(statusManager.getStatus());
    }
};
}  // namespace qmpc::Job
