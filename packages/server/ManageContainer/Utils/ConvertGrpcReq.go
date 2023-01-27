package utils

import (
	pb_l2m "github.com/acompany-develop/QuickMPC/proto/LibcToManageContainer"
	pb_m2c "github.com/acompany-develop/QuickMPC/proto/ManageToComputationContainer"
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

func ConvertPredictRequest(in *pb_l2m.PredictRequest) *pb_m2c.PredictRequest {
	out := &pb_m2c.PredictRequest{
		JobUuid:           in.GetJobUuid(),
		ModelParamJobUuid: in.GetModelParamJobUuid(),
		ModelId:           in.GetModelId(),
		Table: &pb_m2c.JoinOrder{
			DataIds: in.GetTable().GetDataIds(),
			Join:    in.GetTable().GetJoin(),
			Index:   in.GetTable().GetIndex()},
		Src: in.GetSrc(),
	}
	return out
}
