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

#include "client/computation_to_db/join_table.hpp"
#include "external/proto/manage_to_computation_container/manage_to_computation.grpc.pb.h"
#include "job_parameter.hpp"
#include "job_status.hpp"
#include "logging/logger.hpp"
#include "share/share.hpp"

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

    StatusManager statusManager;

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
        // prejob
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

        // readDB
        statusManager.nextStatus();
        auto table = qmpc::ComputationToDb::readTable(request.table());

        // compute
        statusManager.nextStatus();
        validate_cols(table.getSchemas(), arg);
        static_cast<T *>(this)->compute(request.job_uuid(), table, arg);

        // completed
        statusManager.nextStatus();

        return static_cast<int>(statusManager.getStatus());
    }
};
}  // namespace qmpc::Job
