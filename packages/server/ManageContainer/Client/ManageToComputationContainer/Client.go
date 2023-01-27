package mng2comp

import (
	"context"
	"fmt"
	"time"

	"google.golang.org/grpc"
	"google.golang.org/grpc/codes"
	"google.golang.org/grpc/health/grpc_health_v1"
	"google.golang.org/grpc/status"

	helper "github.com/acompany-develop/QuickMPC/packages/server/ManageContainer/Client/Helper"
	datastore "github.com/acompany-develop/QuickMPC/packages/server/ManageContainer/DataStore"
	. "github.com/acompany-develop/QuickMPC/packages/server/ManageContainer/Log"
	utils "github.com/acompany-develop/QuickMPC/packages/server/ManageContainer/Utils"
	pb "github.com/acompany-develop/QuickMPC/proto/ManageToComputationContainer"
	pb_types "github.com/acompany-develop/QuickMPC/proto/common_types"
)

type Client struct{}
type M2CClient interface {
	ExecuteComputation(*pb.ExecuteComputationRequest) (string, int32, error)
	Predict(*pb.PredictRequest) (string, int32, error)
	CheckProgress(string) (*pb_types.JobProgress, error)
}

// CCへのconnecterを得る
func connect() (*grpc.ClientConn, error) {
	config, err := utils.GetConfig()
	computeIP := config.Containers.Computation
	if err != nil {
		return nil, err
	}
	conn, err := grpc.Dial(computeIP.Host, grpc.WithInsecure())
	if err != nil {
		return nil, fmt.Errorf("did not connect: %v", err)
	}
	return conn, nil
}

// (conn)に計算リクエストを送る
func executeComputation(conn *grpc.ClientConn, req *pb.ExecuteComputationRequest) (string, int32, error) {
	client := pb.NewManageToComputationClient(conn)

	rm := helper.RetryManager{}
	for {
		_, err := client.ExecuteComputation(context.TODO(), req)
		retry, _ := rm.Retry(err)
		if !retry {
			st, _ := status.FromError(err)
			if st.Code() == codes.OK {
				return "ok", int32(codes.OK), nil
			} else {
				return st.Message(), int32(st.Code()), err
			}
		}
	}
}

// CCに計算リクエストを送る
func (Client) ExecuteComputation(req *pb.ExecuteComputationRequest) (string, int32, error) {
	conn, err := connect()
	if err != nil {
		return "", 0, err
	}
	defer conn.Close()
	return executeComputation(conn, req)
}

// 定期的にCCの状態を確認する
func CheckStateOfComputationContainerRegularly() {
	timeTicker := time.NewTicker(time.Minute * 1)
	defer timeTicker.Stop()

	for {
		select {
		case <-timeTicker.C:
			checkStateOfComputationContainer()
		}
	}
}

// (conn)に状態確認リクエストを送る
func checkState(conn *grpc.ClientConn) (grpc_health_v1.HealthCheckResponse_ServingStatus, error) {
	client := grpc_health_v1.NewHealthClient(conn)
	res, err := client.Check(context.TODO(), &grpc_health_v1.HealthCheckRequest{})
	return res.GetStatus(), err
}

// CCの状態を確認する
func checkStateOfComputationContainer() {
	conn, err := connect()
	if err != nil {
		AppLogger.Error(err)
	}
	defer conn.Close()

	state, err := checkState(conn)
	if err != nil || state != grpc_health_v1.HealthCheckResponse_SERVING {
		AppLogger.Error("ComputationContainer is not working.")
		AppLogger.Error(err)
		AppLogger.Error(state)
		// 応答なし
		datastore.StateOfComputationContainer = 2
	} else {
		AppLogger.Info("ComputationContainer is working.")
		datastore.StateOfComputationContainer = 1
	}
}

// (conn)にモデル値予測リクエストを送る
func predict(conn *grpc.ClientConn, req *pb.PredictRequest) (string, int32, error) {
	client := pb.NewManageToComputationClient(conn)
	rm := helper.RetryManager{}
	for {
		_, err := client.Predict(context.TODO(), req)
		retry, _ := rm.Retry(err)
		if !retry {
			st, _ := status.FromError(err)
			if st.Code() == codes.OK {
				return "ok", int32(codes.OK), nil
			} else {
				return st.Message(), int32(st.Code()), err
			}
		}
	}
}

// CCにモデル値予測リクエストを送る
func (Client) Predict(req *pb.PredictRequest) (string, int32, error) {
	conn, err := connect()
	if err != nil {
		return "", 0, err
	}
	defer conn.Close()
	return predict(conn, req)
}

func checkProgress(conn *grpc.ClientConn, req *pb.CheckProgressRequest) (*pb_types.JobProgress, error) {
	client := pb.NewManageToComputationClient(conn)

	res, err := client.CheckProgress(context.TODO(), req)

	if err != nil {
		return nil, err
	}

	return res, nil
}

func (Client) CheckProgress(jobUUID string) (*pb_types.JobProgress, error) {
	conn, err := connect()
	if err != nil {
		return nil, err
	}
	defer conn.Close()
	return checkProgress(conn, &pb.CheckProgressRequest{
		JobUuid: jobUUID,
	})
}
