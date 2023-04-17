package utils

import (
	pb_l2m "github.com/acompany-develop/QuickMPC/proto/libc_to_manage_container"
	pb_l2m_v0 "github.com/acompany-develop/QuickMPC/proto/libc_to_manage_container_v0"
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

func ConvertExecuteComputationRequest_v0(in *pb_l2m_v0.ExecuteComputationRequest, jobUUID string) *pb_m2c.ExecuteComputationRequest {
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