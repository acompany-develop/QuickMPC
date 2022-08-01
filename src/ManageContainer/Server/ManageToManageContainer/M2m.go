package m2mserver

import (
	"context"

	m2dg "github.com/acompany-develop/QuickMPC/src/ManageContainer/Client/ManageToDbGate"
	. "github.com/acompany-develop/QuickMPC/src/ManageContainer/Log"
	common "github.com/acompany-develop/QuickMPC/src/ManageContainer/Server"
	utils "github.com/acompany-develop/QuickMPC/src/ManageContainer/Utils"
	pb "github.com/acompany-develop/QuickMPC/src/Proto/ManageToManageContainer"
)

type server struct {
	pb.UnimplementedManageToManageServer
	m2dbclient m2dg.M2DbClient
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

func RunServer() {
	config, err := utils.GetConfig()
	if err != nil {
		AppLogger.Fatalf("failed to parse Config: %v", err)
	}
	partyID := config.PartyID
	ip := config.Containers.PartyList[partyID-1].IpAddress
	lis, port := common.Listen(ip)

	s := common.NewServer()
	pb.RegisterManageToManageServer(s, &server{m2dbclient: m2dg.Client{}})

	AppLogger.Info("M2m Server listening on", port)
	if err := s.Serve(lis); err != nil {
		AppLogger.Fatalf("failed to serve: %v", err)
	}
}
