#pragma once

#include "client/computation_to_db/value_table.hpp"
#include "external/proto/manage_to_computation_container/manage_to_computation.grpc.pb.h"

namespace qmpc::ComputationToDb
{
ValueTable vjoin(const ValueTable &, const ValueTable &, int, int);
ValueTable hjoin(const ValueTable &, const ValueTable &, int, int);
ValueTable hjoinShare(const ValueTable &, const ValueTable &, int, int);

ValueTable readTable(const managetocomputation::JoinOrder &);
}  // namespace qmpc::ComputationToDb
