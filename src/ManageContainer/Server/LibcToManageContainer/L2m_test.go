package l2mserver

import (
	"context"
	"fmt"
	"io"
	"reflect"
	"testing"

	m2db "github.com/acompany-develop/QuickMPC/src/ManageContainer/Client/ManageToDb"
	utils "github.com/acompany-develop/QuickMPC/src/ManageContainer/Utils"
	pb "github.com/acompany-develop/QuickMPC/src/Proto/LibcToManageContainer"
	pb_m2c "github.com/acompany-develop/QuickMPC/src/Proto/ManageToComputationContainer"
	pb_types "github.com/acompany-develop/QuickMPC/src/Proto/common_types"
)

// Test用のDbGとCCのmock
type localDb struct{}
type localCC struct{}
type localMC struct{}
type localTokenCA struct{}

func (localDb) InsertShares(string, []string, int32, string, string, int32) error {
	return nil
}
func (localDb) DeleteShares([]string) error {
	return nil
}
func (localDb) GetSchema(string) ([]string, error) {
	return []string{"attr1"}, nil
}
func (localDb) GetComputationResult(string, []string) ([]*m2db.ComputationResult, *pb_types.JobErrorInfo, error) {
	return []*m2db.ComputationResult{{Result: []string{"result"}}, {Result: []string{"result"}}}, nil, nil
}
func (localDb) InsertModelParams(string, []string, int32) error {
	return nil
}
func (localDb) GetDataList() (string, error) {
	return "result", nil
}
func (localCC) ExecuteComputation(*pb_m2c.ExecuteComputationRequest) (string, int32, error) {
	return "", 0, nil
}
func (localCC) Predict(*pb_m2c.PredictRequest) (string, int32, error) {
	return "", 0, nil
}
func (localCC) CheckProgress(string) (*pb_types.JobProgress, error) {
	return nil, nil
}
func (localMC) DeleteShares(string) error {
	return nil
}
func (localMC) Sync(string) error {
	return nil
}
func (localDb) GetElapsedTime(string) (float64, error) {
	return 0, nil
}
func (localDb) GetMatchingColumn(string) (int32, error) {
	return 1, nil
}
func (localTokenCA) AuthorizeDep(token string) error {
	return nil
}
func (localTokenCA) AuthorizeDemo(token string) error {
	return nil
}

// Test用のサーバを起動(libClient)
var s *utils.TestServer

func init() {
	s = &utils.TestServer{}
	pb.RegisterLibcToManageServer(s.GetServer(), &server{m2dbclient: localDb{}, m2cclient: localCC{}, m2mclient: localMC{}, m2tclient: localTokenCA{}})
	s.Serve()
}

// schemaが得れるかのTest
func TestGetSchema(t *testing.T) {
	conn := s.GetConn()
	defer conn.Close()

	client := pb.NewLibcToManageClient(conn)

	result, err := client.GetSchema(context.Background(), &pb.GetSchemaRequest{
		DataId: "id", Token: "token"})

	if err != nil {
		t.Fatal(err)
	}
	schema := result.GetSchema()
	if schema[0] != "attr1" {
		t.Fatal("GetScheme Failed")
	}
}

// 計算リクエストが送れるかTest
func TestExecuteComputation(t *testing.T) {
	conn := s.GetConn()
	defer conn.Close()

	client := pb.NewLibcToManageClient(conn)

	_, err := client.ExecuteComputation(context.Background(), &pb.ExecuteComputationRequest{
		MethodId: 1,
		Table: &pb.JoinOrder{
			DataIds: []string{"id"},
			Join:    []int32{},
			Index:   []int32{1}},
		Arg: &pb.Input{
			Src:    []int32{},
			Target: []int32{}}})
	if err != nil {
		t.Fatal(err)
	}
}

// id列が異なっていた場合にエラーがでるかテスト
func TestExecuteComputationFailedDifferentIdColumn(t *testing.T) {
	conn := s.GetConn()
	defer conn.Close()

	client := pb.NewLibcToManageClient(conn)

	_, err := client.ExecuteComputation(context.Background(), &pb.ExecuteComputationRequest{
		MethodId: 1,
		Table: &pb.JoinOrder{
			DataIds: []string{"id"},
			Join:    []int32{},
			Index:   []int32{2}},
		Arg: &pb.Input{
			Src:    []int32{},
			Target: []int32{}}})
	if err == nil {
		t.Error("exucute computation must be failed, but success.")
	}
}

// 計算結果を取得できるかTest
func TestGetComputationResult(t *testing.T) {
	conn := s.GetConn()
	defer conn.Close()
	client := pb.NewLibcToManageClient(conn)

	stream, err := client.GetComputationResult(context.Background(), &pb.GetComputationResultRequest{JobUuid: "id", Token: "token"})

	if err != nil {
		t.Fatal(err)
	}

	for {
		reply, err := stream.Recv()
		if err == io.EOF {
			break
		}
		if err != nil {
			t.Fatal(err)
		}

		res := reply.GetResult()
		ac := []string{"result"}
		if !reflect.DeepEqual(res, ac) {
			t.Fatal(fmt.Sprintf("GetComputationResult Failed. getResult() must be %s, but response is %s", ac, res))
		}
	}
}

// モデル予測値を取得できるかTest
func TestPredict(t *testing.T) {
	conn := s.GetConn()
	defer conn.Close()
	client := pb.NewLibcToManageClient(conn)
	_, err := client.Predict(context.Background(), &pb.PredictRequest{
		JobUuid: "id",
		ModelId: 1,
		Table:   &pb.JoinOrder{DataIds: []string{"id"}, Join: []int32{}, Index: []int32{1}},
		Src:     []int32{}})

	if err != nil {
		t.Fatal(err)
	}
}

// id列が異なる場合にエラーがでるかTest
func TestPredictFailedDifferentIdColumn(t *testing.T) {
	conn := s.GetConn()
	defer conn.Close()
	client := pb.NewLibcToManageClient(conn)
	_, err := client.Predict(context.Background(), &pb.PredictRequest{
		JobUuid: "id",
		ModelId: 1,
		Table:   &pb.JoinOrder{DataIds: []string{"id"}, Join: []int32{}, Index: []int32{2}},
		Src:     []int32{}})

	if err == nil {
		t.Error("predict must be failed, but success.")
	}
}

func TestSendModelParam(t *testing.T) {
	conn := s.GetConn()
	defer conn.Close()
	client := pb.NewLibcToManageClient(conn)

	result, err := client.SendModelParam(context.Background(), &pb.SendModelParamRequest{
		JobUuid: "id",
		Params:  []string{"1", "2"},
		Token:   "token"})

	if err != nil {
		t.Fatal(err)
	}
	if !result.IsOk {
		t.Fatal("Send Model Parameters Failed")
	}
}

func TestGetDataList(t *testing.T) {

	conn := s.GetConn()
	defer conn.Close()
	client := pb.NewLibcToManageClient(conn)

	result, err := client.GetDataList(context.Background(), &pb.GetDataListRequest{
		Token: "token_dep",
	})

	if err != nil {
		t.Fatal(err)
	}

	if result.GetResult() != "result" {
		t.Fatal("GetDataList Failed")
	}
}

func TestGetElapsedTime(t *testing.T) {

	conn := s.GetConn()
	defer conn.Close()
	client := pb.NewLibcToManageClient(conn)

	result, err := client.GetElapsedTime(context.Background(), &pb.GetElapsedTimeRequest{
		JobUuid: "id",
		Token:   "token_dep",
	})

	if err != nil {
		t.Fatal(err)
	}

	if result.GetElapsedTime() != 0 {
		t.Fatal("GetElapsedTime Failed")
	}
}
