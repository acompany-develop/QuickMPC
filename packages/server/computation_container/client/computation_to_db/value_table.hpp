#pragma once

#include <optional>
#include <string>
#include <vector>

#include "client/computation_to_db/client.hpp"
#include "client/computation_to_db/value_table_new.hpp"
#include "external/proto/manage_to_computation_container/manage_to_computation.grpc.pb.h"

namespace qmpc::ComputationToDb
{
class TableJoiner
{
    const std::string data_id;

    std::vector<std::vector<std::string>>
    getSubTable(const std::optional<std::vector<int>> &, const std::optional<std::vector<int>> &)
        const;
    std::vector<std::string> getColumn(int) const;
    std::string joinDataId(const TableJoiner &, int) const;

public:
    TableJoiner(const std::string &);

    std::string getDataId() const;
    std::vector<std::vector<std::string>> getTable() const;
    std::vector<std::string> getSchemas() const;
    TableJoiner vjoin(const TableJoiner &, int, int) const;
    TableJoiner hjoin(const TableJoiner &, int, int) const;
    TableJoiner hjoinShare(const TableJoiner &, int, int) const;
};

ValueTable vjoin(const ValueTable &, const ValueTable &, int, int);
ValueTable hjoin(const ValueTable &, const ValueTable &, int, int);
ValueTable hjoinShare(const ValueTable &, const ValueTable &, int, int);

ValueTable readTable(const managetocomputation::JoinOrder &);
}  // namespace qmpc::ComputationToDb
