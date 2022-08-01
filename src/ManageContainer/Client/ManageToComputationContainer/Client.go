package mng2comp

import (
	"context"
	"fmt"
	"time"

	"google.golang.org/grpc"
	"google.golang.org/grpc/health/grpc_health_v1"
	"google.golang.org/grpc/status"

	datastore "github.com/acompany-develop/QuickMPC/src/ManageContainer/DataStore"
	. "github.com/acompany-develop/QuickMPC/src/ManageContainer/Log"
	utils "github.com/acompany-develop/QuickMPC/src/ManageContainer/Utils"
	pb "github.com/acompany-develop/QuickMPC/src/Proto/ManageToComputationContainer"
)

type Client struct{}
type M2CClient interface {
	ExecuteComputation(*pb.ExecuteComputationRequest) (string, int32, error)
	Predict(*pb.PredictRequest) (string, int32, error)
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

	_, err := client.ExecuteComputation(context.TODO(), req)
	//CC側からエラーが返却された場合
	if err != nil {
		st, ok := status.FromError(err)
		if !ok {
			AppLogger.Errorf("GRPC Error : Code [%d], Message [%s]", st.Code(), st.Message())
		}
		return st.Message(), int32(st.Code()), err
	}
	return "ok", 0, nil
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

	_, err := client.Predict(context.TODO(), req)
	//CC側からエラーが返却された場合
	if err != nil {
		st, ok := status.FromError(err)
		if !ok {
			AppLogger.Errorf("GRPC Error : Code [%d], Message [%s]", st.Code(), st.Message())
		}
		return st.Message(), int32(st.Code()), err
	}
	return "ok", 0, nil
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
