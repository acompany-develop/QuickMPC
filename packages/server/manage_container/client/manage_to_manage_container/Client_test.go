package mng2mng

import (
	"context"
	"testing"

	. "github.com/acompany-develop/QuickMPC/packages/server/manage_container/Log"
	utils "github.com/acompany-develop/QuickMPC/packages/server/manage_container/Utils"
	pb "github.com/acompany-develop/QuickMPC/proto/ManageToManageContainer"
)

// Test用のMCのmock
type server struct {
	pb.UnsafeManageToManageServer
}

func (s *server) DeleteShares(ctx context.Context, in *pb.DeleteSharesRequest) (*pb.DeleteSharesResponse, error) {
	AppLogger.Infof("Received: %v", in.GetDataId())
	return &pb.DeleteSharesResponse{}, nil
}

func (s *server) Sync(ctx context.Context, in *pb.SyncRequest) (*pb.SyncResponse, error) {
	return &pb.SyncResponse{}, nil
}

// Test用のサーバを起動(MC)
var s *utils.TestServer
var c = Client{}

func init() {
	s = &utils.TestServer{}
	pb.RegisterManageToManageServer(s.GetServer(), &server{})
	s.Serve()
}

func TestDeleteShares(t *testing.T) {
	conn := s.GetConn()
	defer conn.Close()
	err := c.deleteShares(conn, "dataID")
	if err != nil {
		t.Error("delete shares faild: " + err.Error())
	}
}

func TestSync(t *testing.T) {
	conn := s.GetConn()
	defer conn.Close()
	err := c.sync(conn, "SyncID")
	if err != nil {
		t.Error("sync faild: " + err.Error())
	}
}
