#pragma once

#include <string>
#include <vector>

#include "external/proto/manage_to_computation_container/manage_to_computation.grpc.pb.h"
#include "share/share.hpp"

namespace qmpc::ComputationToDb
{
class ValueTable
{
    using Share = ::Share;
    const std::string data_id;

    std::vector<Share> getColumn(int) const;
    std::string joinDataId(const ValueTable &vt) const;

public:
    ValueTable(const std::string &);

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
