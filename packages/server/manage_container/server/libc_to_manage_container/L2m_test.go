package l2mserver

import (
	"context"
	"fmt"
	"io"
	"reflect"
	"testing"

	utils "github.com/acompany-develop/QuickMPC/packages/server/manage_container/utils"
	pb "github.com/acompany-develop/QuickMPC/proto/libc_to_manage_container"
)

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
		DataId: exist_data_id, Token: "token"})

	if err != nil {
		t.Fatal(err)
	}
	schema := result.GetSchema()
	if schema[0].Name != "attr1" {
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
			DataIds:   []string{exist_data_id},
			DebugMode: true,
		},
		Arg: &pb.Input{
			Src:    []int32{},
			Target: []int32{}}})
	if err != nil {
		t.Fatal(err)
	}
}

// 計算結果を取得できるかTest
func TestGetComputationResult(t *testing.T) {
	conn := s.GetConn()
	defer conn.Close()
	client := pb.NewLibcToManageClient(conn)

	stream, err := client.GetComputationResult(context.Background(), &pb.GetComputationRequest{JobUuid: "id", Token: "token"})

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

// エラー情報を取得できるかTest
func TestGetJobErrorInfo(t *testing.T) {
	conn := s.GetConn()
	defer conn.Close()
	client := pb.NewLibcToManageClient(conn)

	result, err := client.GetJobErrorInfo(context.Background(), &pb.GetJobErrorInfoRequest{
		JobUuid: "id",
		Token:   "token_dep",
	})

	if err != nil {
		t.Fatal(err)
	}

	if result.GetJobErrorInfo().GetWhat() != "test" {
		t.Fatal("GetJobErrorInfo Failed")
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

func TestAddShareDataFrame(t *testing.T) {
	conn := s.GetConn()
	defer conn.Close()
	client := pb.NewLibcToManageClient(conn)

	_, err := client.AddShareDataFrame(context.Background(), &pb.AddShareDataFrameRequest{
		BaseDataId: exist_data_id,
		AddDataId:  exist_data_id,
		Token:      "token_dep",
	})

	if err != nil {
		t.Fatal(err)
	}
}
