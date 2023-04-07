package mng2mng

import (
	"context"
	"testing"

	. "github.com/acompany-develop/QuickMPC/packages/server/manage_container/log"
	utils "github.com/acompany-develop/QuickMPC/packages/server/manage_container/utils"
	pb "github.com/acompany-develop/QuickMPC/proto/manage_to_manage_container"
	empty "github.com/golang/protobuf/ptypes/empty"
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

func (s *server) CreateStatusFile(ctx context.Context, in *pb.CreateStatusFileRequest) (*empty.Empty, error) {
	AppLogger.Infof("Received: %v", in.GetJobUuid())
	return &empty.Empty{}, nil
}

func (s *server) DeleteStatusFile(ctx context.Context, in *pb.DeleteStatusFileRequest) (*empty.Empty, error) {
	AppLogger.Infof("Received: %v", in.GetJobUuid())
	return &empty.Empty{}, nil
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

func TestCreateStatusFile(t *testing.T) {
	conn := s.GetConn()
	defer conn.Close()
	err := c.createStatusFile(conn, "jobUUID")
	if err != nil {
		t.Error("create status file faild: " + err.Error())
	}
}

func TestDeleteStatusFile(t *testing.T) {
	conn := s.GetConn()
	defer conn.Close()
	err := c.deleteStatusFile(conn, "jobUUID")
	if err != nil {
		t.Error("delete status file faild: " + err.Error())
	}
}
