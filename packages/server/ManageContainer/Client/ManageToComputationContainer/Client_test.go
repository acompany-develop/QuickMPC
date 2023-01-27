package mng2comp

import (
	"context"
	"testing"
	"time"

	"google.golang.org/grpc"

	. "github.com/acompany-develop/QuickMPC/src/ManageContainer/Log"
	utils "github.com/acompany-develop/QuickMPC/src/ManageContainer/Utils"
	pb "github.com/acompany-develop/QuickMPC/src/proto/ManageToComputationContainer"
	empty "github.com/golang/protobuf/ptypes/empty"
)

// Test用のCCのmock
type server struct {
	pb.UnimplementedManageToComputationServer
}

func (s *server) ExecuteComputation(ctx context.Context, in *pb.ExecuteComputationRequest) (*empty.Empty, error) {
	AppLogger.Infof("Received: %v", in.GetMethodId())
	AppLogger.Infof("Received: %v", in.GetJobUuid())
	return &empty.Empty{}, nil
}
func (s *server) CheckState(ctx context.Context, in *empty.Empty) (*pb.CheckStateResponse, error) {
	return &pb.CheckStateResponse{State: 1}, nil
}
func (s *server) Predict(ctx context.Context, in *pb.PredictRequest) (*empty.Empty, error) {
	AppLogger.Infof("Received: %v", in.GetJobUuid())
	AppLogger.Infof("Received: %v", in.GetModelId())
	return &empty.Empty{}, nil
}

// Test用のサーバを起動(CC)
var s *utils.TestServer

func init() {
	s = &utils.TestServer{}
	pb.RegisterManageToComputationServer(s.GetServer(), &server{})
	s.Serve()
}

// 計算リクエストが遅れるかTest
func TestExecuteComputation(t *testing.T) {
	conn := s.GetConn()
	defer conn.Close()

	req := &pb.ExecuteComputationRequest{
		MethodId: 1,
		JobUuid:  "hugahuga",
		Table: &pb.JoinOrder{
			DataIds: []string{"hoge", "huga"},
			Join:    []int32{0},
		},
		Arg: &pb.Input{
			Src:    []int32{0, 1},
			Target: []int32{0},
		},
	}

	message, status, err := executeComputation(conn, req)
	if err != nil {
		t.Fatal(err)
	}

	if message != "ok" {
		t.Fatalf("ExecuteComputation must return message 'ok'. but message is '%s'", message)
	}

	if status != 0 {
		t.Fatalf("ExecuteComputation must return status '0'. but status is '%d'", status)
	}
}

// 状態確認ができるかTest
func TestCheckState(t *testing.T) {
	conn := s.GetConn()
	defer conn.Close()

	status, err := checkState(conn)
	if err != nil {
		t.Fatal(err)
	}

	if status != 1 {
		t.Fatal("ExecuteComputation must return status '1'")
	}
}
func CheckExecuteComputation() {

	timeTicker := time.NewTicker(time.Minute * 1)
	defer timeTicker.Stop()
	for {
		select {
		case <-timeTicker.C:

			config, err := utils.GetConfig()
			if err != nil {
				AppLogger.Fatalf("Config読み込みエラー: %v", err)
			}
			computeIp := config.Containers.Computation
			conn, err := grpc.Dial(computeIp.Host, grpc.WithInsecure())
			if err != nil {
				AppLogger.Fatalf("did not connect: %v", err)
			}
			req := &pb.ExecuteComputationRequest{
				MethodId: 1,
				JobUuid:  "jobUuid",
				Table: &pb.JoinOrder{
					DataIds: []string{"apple", "banana"},
					Join:    []int32{0},
				},
				Arg: &pb.Input{
					Src:    []int32{0, 1},
					Target: []int32{0},
				},
			}

			message, status, _ := executeComputation(conn, req)
			AppLogger.Info(message)
			AppLogger.Info(status)
		}
	}

}

// モデル値予測リクエストが遅れるかTest
func TestPredict(t *testing.T) {
	conn := s.GetConn()
	defer conn.Close()

	req := &pb.PredictRequest{
		JobUuid: "hugahuga",
		ModelId: 1,
		Table:   &pb.JoinOrder{DataIds: []string{"id"}, Join: []int32{}},
		Src:     []int32{}}

	message, status, err := predict(conn, req)
	if err != nil {
		t.Fatal(err)
	}

	if message != "ok" {
		t.Fatalf("Result must return message 'ok'. but message is '%s'", message)
	}

	if status != 0 {
		t.Fatalf("Result must return status '0'. but status is '%d'", status)
	}
}
