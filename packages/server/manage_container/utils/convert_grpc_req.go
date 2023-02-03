package utils

import (
	pb_l2m "github.com/acompany-develop/QuickMPC/proto/libc_to_manage_container"
	pb_m2c "github.com/acompany-develop/QuickMPC/proto/manage_to_computation_container"
)

func ConvertExecuteComputationRequest(in *pb_l2m.ExecuteComputationRequest, jobUUID string) *pb_m2c.ExecuteComputationRequest {
	out := &pb_m2c.ExecuteComputationRequest{
		MethodId: in.GetMethodId(),
		JobUuid:  jobUUID,
		Table: &pb_m2c.JoinOrder{
			DataIds: in.GetTable().GetDataIds(),
			Join:    in.GetTable().GetJoin(),
			Index:   in.GetTable().GetIndex()},
		Arg: &pb_m2c.Input{
			Src:    in.GetArg().GetSrc(),
			Target: in.GetArg().GetTarget()}}
	return out
}
