package m2mserver

import (
	"context"

	m2db "github.com/acompany-develop/QuickMPC/packages/server/manage_container/client/manage_to_db"
	. "github.com/acompany-develop/QuickMPC/packages/server/manage_container/log"
	common "github.com/acompany-develop/QuickMPC/packages/server/manage_container/server"
	utils "github.com/acompany-develop/QuickMPC/packages/server/manage_container/utils"
	pb "github.com/acompany-develop/QuickMPC/proto/manage_to_manage_container"
	empty "github.com/golang/protobuf/ptypes/empty"
)

type server struct {
	pb.UnimplementedManageToManageServer
	m2dbclient m2db.M2DbClient
}

func (s *server) DeleteShares(ctx context.Context, in *pb.DeleteSharesRequest) (*pb.DeleteSharesResponse, error) {
	dataID := in.GetDataId()
	AppLogger.Infof("delete share request: dataID = %s", dataID)

	err := s.m2dbclient.DeleteShares([]string{dataID})
	if err != nil {
		AppLogger.Error(err)
		return &pb.DeleteSharesResponse{
			Message: err.Error(),
			IsOk:    false,
		}, nil
	}
	return &pb.DeleteSharesResponse{
		Message: "ok",
		IsOk:    true,
	}, nil
}

func (s *server) Sync(ctx context.Context, in *pb.SyncRequest) (*pb.SyncResponse, error) {
	syncID := in.SyncId
	increment(syncID)
	return &pb.SyncResponse{
		Message: "ok",
		IsOk:    true,
	}, nil
}

func (s *server) CreateStatusFile(ctx context.Context, in *pb.CreateStatusFileRequest) (*empty.Empty, error) {
	jobUUID := in.JobUuid
	AppLogger.Infof("create status file request: jobUUID = %s", jobUUID)
	err := s.m2dbclient.CreateStatusFile(jobUUID)
	return &empty.Empty{}, err
}

func (s *server) DeleteStatusFile(ctx context.Context, in *pb.DeleteStatusFileRequest) (*empty.Empty, error) {
	jobUUID := in.JobUuid
	AppLogger.Infof("delete status file request: jobUUID = %s", jobUUID)
	err := s.m2dbclient.DeleteStatusFile(jobUUID)
	return &empty.Empty{}, err
}

func RunServer() {
	config, err := utils.GetConfig()
	if err != nil {
		AppLogger.Fatalf("failed to parse config: %v", err)
	}
	partyID := config.PartyID
	ip := config.Containers.PartyList[partyID-1].IpAddress
	lis, port := common.Listen(ip)

	s := common.NewServer()
	pb.RegisterManageToManageServer(s, &server{m2dbclient: m2db.Client{}})

	AppLogger.Info("M2m Server listening on", port)
	if err := s.Serve(lis); err != nil {
		AppLogger.Fatalf("failed to serve: %v", err)
	}
}
