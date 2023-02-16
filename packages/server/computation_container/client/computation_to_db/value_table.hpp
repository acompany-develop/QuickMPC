#pragma once

#include <optional>
#include <string>
#include <vector>

#include "external/proto/manage_to_computation_container/manage_to_computation.grpc.pb.h"

namespace qmpc::ComputationToDb
{
class ValueTable
{
    const std::string data_id;

    std::vector<std::vector<std::string>>
    getSubTable(const std::optional<std::vector<int>> &, const std::optional<std::vector<int>> &)
        const;
    std::vector<std::string> getColumn(int) const;
    std::string joinDataId(const ValueTable &, int) const;

public:
    ValueTable(const std::string &);

    std::string getDataId() const;
    std::vector<std::vector<std::string>> getTable() const;
    std::vector<std::string> getSchemas() const;
    ValueTable vjoin(const ValueTable &, int, int) const;
    ValueTable hjoin(const ValueTable &, int, int) const;
    ValueTable hjoinShare(const ValueTable &, int, int) const;
};

ValueTable
parseRead(const std::vector<ValueTable> &, const std::vector<int> &, const std::vector<int> &);
ValueTable readTable(const managetocomputation::JoinOrder &);
}  // namespace qmpc::ComputationToDb
