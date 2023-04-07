package m2mserver

import (
	"context"
	"testing"

	m2db "github.com/acompany-develop/QuickMPC/packages/server/manage_container/client/manage_to_db"
	utils "github.com/acompany-develop/QuickMPC/packages/server/manage_container/utils"
	pb_types "github.com/acompany-develop/QuickMPC/proto/common_types"
	pb "github.com/acompany-develop/QuickMPC/proto/manage_to_manage_container"
)

// Test用のDbGとCCのmock
type localDb struct{}

func (localDb) InsertShares(string, []*pb_types.Schema, int32, string, string, int32) error {
	return nil
}
func (localDb) DeleteShares([]string) error {
	return nil
}
func (localDb) GetSchema(string) ([]*pb_types.Schema, error) {
	return []*pb_types.Schema{{Name: ""}}, nil
}
func (localDb) GetJobErrorInfo(string) (*pb_types.JobErrorInfo, error) {
	return &pb_types.JobErrorInfo{}, nil
}
func (localDb) GetComputationResult(string, []string) ([]*m2db.ComputationResult, *pb_types.JobErrorInfo, error) {
	return []*m2db.ComputationResult{{Result: []string{"result"}}, {Result: []string{"result"}}}, nil, nil
}
func (localDb) InsertModelParams(string, []string, int32) error {
	return nil
}
func (localDb) CreateStatusFile(string) error {
	return nil
}
func (localDb) DeleteStatusFile(string) error {
	return nil
}
func (localDb) GetDataList() (string, error) {
	return "result", nil
}
func (localDb) GetElapsedTime(string) (float64, error) {
	return 0, nil
}
func (localDb) GetMatchingColumn(string) (int32, error) {
	return 1, nil
}

// Test用のサーバを起動(MC)
var s *utils.TestServer

func init() {
	s = &utils.TestServer{}
	pb.RegisterManageToManageServer(s.GetServer(), &server{m2dbclient: localDb{}})
	s.Serve()
}

// Share削除リクエストが受け取れるかのTest
func TestGetSchema(t *testing.T) {
	conn := s.GetConn()
	defer conn.Close()

	client := pb.NewManageToManageClient(conn)

	_, err := client.DeleteShares(context.Background(), &pb.DeleteSharesRequest{
		DataId: "id"})

	if err != nil {
		t.Fatal(err)
	}
}

// Syncリクエストが受け取れるかのTest
func TestSync(t *testing.T) {
	conn := s.GetConn()
	defer conn.Close()

	client := pb.NewManageToManageClient(conn)

	_, err := client.Sync(context.Background(), &pb.SyncRequest{SyncId: "SyncID"})
	if err != nil {
		t.Fatal(err)
	}
}
