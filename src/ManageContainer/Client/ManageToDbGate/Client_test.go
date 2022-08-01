package mng2dbgate

import (
	"strings"
	"testing"

	. "github.com/acompany-develop/QuickMPC/src/ManageContainer/Log"
	utils "github.com/acompany-develop/QuickMPC/src/ManageContainer/Utils"
	pb "github.com/acompany-develop/QuickMPC/src/Proto/AnyToDbGate"
)

// Test用のDBGのmock
type server struct {
	pb.UnimplementedAnyToDbGateServer
}

func (s *server) ExecuteQuery(in *pb.ExecuteQueryRequest, stream pb.AnyToDbGate_ExecuteQueryServer) error {
	AppLogger.Infof("Received: %v", in.GetHost())
	AppLogger.Infof("Received: %v", in.GetQuery())
	if strings.Contains(in.GetQuery(), "existID") {
		stream.Send(&pb.ExecuteQueryResponse{
			Result:  "[{\"data_id\":\"existID\",\"meta\":{\"piece_id\":1,\"schema\":[\"attr1\",\"attr2\",\"attr3\"]},\"job_uuid\":\"test\",\"result\":[\"1\",\"2\",\"3\"]}]",
			PieceId: int32(1),
		})
	} else {
		stream.Send(&pb.ExecuteQueryResponse{
			Result:  "[]",
			PieceId: int32(1),
		})
	}
	return nil
}

// Test用のサーバを起動(DBG)
var s *utils.TestServer
var c = Client{}

func init() {
	s = &utils.TestServer{}
	pb.RegisterAnyToDbGateServer(s.GetServer(), &server{})
	s.Serve()
}

// クエリが送れるかTest
func TestExecuteQuery(t *testing.T) {
	conn := s.GetConn()
	defer conn.Close()
	_, err := ExecuteQuery(conn, "query_string")
	if err != nil {
		t.Error("execute query faild: " + err.Error())
	}
}

// シェアが送れるかTest
func TestInsertShares(t *testing.T) {
	conn := s.GetConn()
	defer conn.Close()
	client := Client{}
	// share送信
	const dataID string = "notExistID"
	schema := []string{"attr1", "attr2", "attr3"}
	const pieceID int32 = 1
	const shares string = "[[\"1\",\"2\",\"3\"],[\"4\",\"5\",\"6\"]]"
	const sent_at string = ""
	err := client.insertShares(conn, dataID, schema, pieceID, shares, sent_at)
	if err != nil {
		t.Error("insert shares faild: " + err.Error())
	}
}

// 重複シェアを弾くかTest
func TestInsertSharesDuplicate(t *testing.T) {
	conn := s.GetConn()
	defer conn.Close()
	client := Client{}
	// share送信
	const dataID string = "existID"
	schema := []string{"attr1", "attr2", "attr3"}
	const pieceID int32 = 1
	const shares string = "[[\"1\",\"2\",\"3\"],[\"4\",\"5\",\"6\"]]"
	const sent_at string = ""
	err := client.insertShares(conn, dataID, schema, pieceID, shares, sent_at)
	if err == nil {
		t.Error("insert duplicate shares success error")
	}
}

// Schemaが取得できるかTest
func TestGetSchema(t *testing.T) {
	conn := s.GetConn()
	defer conn.Close()
	client := Client{}

	// schema取得して比較
	schema, err := client.getSchema(conn, "existID")
	AppLogger.Info(schema)
	if err != nil {
		t.Error("get schema failed: " + err.Error())
	}
}

// 計算結果が取得できるかTest
func TestGetComputationResult(t *testing.T) {
	conn := s.GetConn()
	defer conn.Close()
	client := Client{}
	_, err := client.getComputationResult(conn, "existID")
	if err != nil {
		t.Error("get computation result failed: " + err.Error())
	}
}

func TestDeleteShares(t *testing.T) {
	conn := s.GetConn()
	defer conn.Close()
	client := Client{}

	// シェアが削除できるかTest
	dataIDs := []string{"existID"}
	err := client.deleteShares(conn, dataIDs)
	if err != nil {
		t.Error("delete shares failed: " + err.Error())
	}

	// 指定したdataIDが存在しなければエラーを返すかTest
	dataIDs = []string{"notExistID"}
	err = client.deleteShares(conn, dataIDs)
	if err == nil {
		t.Error("deleteShares should throw error")
	}
}

// モデルパラメータが送れるかTest
func TestInsertModelParams(t *testing.T) {
	conn := s.GetConn()
	defer conn.Close()
	client := Client{}
	// モデルパラメータ送信
	const jobUUID string = "notExistID"
	const params string = "[\"1\",\"2\",\"3\"]"
	err := client.insertModelParams(conn, jobUUID, params)
	if err != nil {
		t.Error("insert model parameters faild: " + err.Error())
	}
}

// 重複モデルパラメータを弾くかTest
func TestInsertModelParamsDuplicate(t *testing.T) {
	conn := s.GetConn()
	defer conn.Close()
	client := Client{}
	// モデルパラメータ送信
	const jobUUID string = "existID"
	const params string = "[\"1\",\"2\",\"3\"]"
	err := client.insertModelParams(conn, jobUUID, params)
	if err == nil {
		t.Error("insert duplicate model parameters success error")
	}
}
