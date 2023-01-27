#pragma once

#include <string>
#include <vector>

#include "Client/ComputationToDb/Client.hpp"
#include "Client/ComputationToDb/ValueTable.hpp"
#include "Job/JobStatus.hpp"
#include "Share/Share.hpp"
#include "external/proto/ManageToComputationContainer/manage_to_computation.grpc.pb.h"

namespace qmpc::Model
{
using Share = qmpc::Share::Share<FixedPoint>;

class ModelBase
{
    qmpc::Job::StatusManager statusManager{};

    virtual std::vector<Share>
    predict(const std::vector<std::vector<Share>> &, const std::vector<std::string> &, const std::list<int> &, const std::string &)
        const = 0;

    qmpc::ComputationToDb::ValueTable
    parseRead(const std::vector<qmpc::ComputationToDb::ValueTable> &, const std::vector<int> &, const std::vector<int> &)
        const;
    std::pair<std::vector<std::vector<Share>>, std::vector<std::string>>
    readDb(const managetocomputation::PredictRequest &);

    static auto toString(const std::vector<Share> &values)
    {
        std::vector<std::string> results;
        results.reserve(values.size());
        for (const auto &value : values)
        {
            results.emplace_back(value.getVal().getStrVal());
        }
        return results;
    }

    template <typename U>
    void writeDb(const managetocomputation::PredictRequest &request, U &&values)
    {
        statusManager.nextStatus();
        auto db_client = qmpc::ComputationToDb::Client::getInstance();
        auto results = toString(values);
        db_client->writeComputationResult(request.job_uuid(), results);
        statusManager.nextStatus();
    }

    static void validate_cols(const std::vector<std::string> &schemas, const std::list<int> &src);

public:
    virtual ~ModelBase() {}

    int run(const managetocomputation::PredictRequest &);
};
}  // namespace qmpc::Model
