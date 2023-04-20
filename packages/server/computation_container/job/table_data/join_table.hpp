#pragma once

#include "external/proto/manage_to_computation_container/manage_to_computation.grpc.pb.h"
#include "job/table_data/value_table.hpp"

namespace qmpc::ComputationToDb
{
ValueTable vjoin(const ValueTable &, const ValueTable &, int, int);
ValueTable hjoin(const ValueTable &, const ValueTable &, int, int);
ValueTable hjoinShare(const ValueTable &, const ValueTable &, int, int);

ValueTable readTable(const managetocomputation::JoinOrder &);
}  // namespace qmpc::ComputationToDb
