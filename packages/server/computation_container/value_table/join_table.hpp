#pragma once

#include "external/proto/manage_to_computation_container/manage_to_computation.grpc.pb.h"
#include "value_table/value_table.hpp"

namespace qmpc::ComputationToDb
{
ValueTable vjoin(const ValueTable &, const ValueTable &);
ValueTable hjoin(const ValueTable &, const ValueTable &);
ValueTable hjoinShare(const ValueTable &, const ValueTable &);

ValueTable readTable(const managetocomputation::JoinOrder &);
}  // namespace qmpc::ComputationToDb
