package e2bserver

import (
	"context"
	"fmt"
	"net"
	"os"
	"strconv"
	"time"

	grpc_middleware "github.com/grpc-ecosystem/go-grpc-middleware"
	grpc_auth "github.com/grpc-ecosystem/go-grpc-middleware/auth"
	"google.golang.org/grpc"
	"google.golang.org/grpc/codes"
	"google.golang.org/grpc/health"
	healthpb "google.golang.org/grpc/health/grpc_health_v1"
	"google.golang.org/grpc/keepalive"
	"google.golang.org/grpc/reflection"
	"google.golang.org/grpc/status"

	jwt_types "github.com/acompany-develop/QuickMPC/packages/server/beaver_triple_service/jwt"
	logger "github.com/acompany-develop/QuickMPC/packages/server/beaver_triple_service/log"
	tg "github.com/acompany-develop/QuickMPC/packages/server/beaver_triple_service/triple_generator"
	utils "github.com/acompany-develop/QuickMPC/packages/server/beaver_triple_service/utils"
	pb "github.com/acompany-develop/QuickMPC/proto/engine_to_bts"
	emptypb "google.golang.org/protobuf/types/known/emptypb"
)

type server struct {
	pb.UnimplementedEngineToBtsServer
}

func getListenPort() (string, error) {
	port, ok := os.LookupEnv("PORT")
	if !ok {
		return "", status.Error(codes.Internal, "port is not provided")
	}
	return port, nil
}

func getWithEnvoy() (bool, error) {
	with_envoy, ok := os.LookupEnv("WITH_ENVOY")
	if !ok {
		return false, status.Error(codes.Internal, "with envoy is not provided")
	}
	return strconv.ParseBool(with_envoy)
}

// モック時に置き換わる関数
var GetPartyIdFromClaims = func(claims *jwt_types.Claim) (uint32, error) {
	return claims.PartyId, nil
}

func (s *server) GetTriples(ctx context.Context, in *pb.GetTriplesRequest) (*pb.GetTriplesResponse, error) {
	claims, ok := ctx.Value("claims").(*jwt_types.Claim)
	if !ok {
		return nil, status.Error(codes.Internal, "failed claims type assertions")
	}

	partyId, err := GetPartyIdFromClaims(claims)
	if err != nil {
		return nil, err
	}
	logger.Infof("jobId: %d, partyId: %d Type: %v\n", in.GetJobId(), partyId, in.GetTripleType())

	triples, err := tg.GetTriples(claims, in.GetJobId(), partyId, in.GetAmount(), in.GetTripleType(), in.GetRequestId())
	if err != nil {
		return nil, err
	}

	return &pb.GetTriplesResponse{
		Triples: triples,
	}, nil
}

func (s *server) DeleteJobIdTriple(ctx context.Context, in *pb.DeleteJobIdTripleRequest) (*emptypb.Empty, error) {
	claims, ok := ctx.Value("claims").(*jwt_types.Claim)
	if !ok {
		return nil, status.Error(codes.Internal, "failed claims type assertions")
	}

	partyId, err := GetPartyIdFromClaims(claims)
	if err != nil {
		return nil, err
	}
	logger.Infof("jobId: %d, partyId: %d\n", in.GetJobId(), partyId)

	err = tg.DeleteJobIdTriple(in.GetJobId())
	if err != nil {
		return nil, err
	}

	return &emptypb.Empty{}, err
}

func RunServer() {
	listenPort, err := getListenPort()
	if err != nil {
		logger.Fatalf("failed to get listen port: %v", err)
	}

	listenIp := fmt.Sprintf("0.0.0.0:%s", listenPort)
	lis, err := net.Listen("tcp", listenIp)
	if err != nil {
		logger.Fatalf("failed to listen: %v", err)
	}

	enforcementPolicyMinTime := 5
	s := grpc.NewServer(
		grpc.KeepaliveEnforcementPolicy(
			keepalive.EnforcementPolicy{
				MinTime:             (time.Duration(enforcementPolicyMinTime) * time.Second),
				PermitWithoutStream: true,
			},
		),
		grpc.UnaryInterceptor(
			grpc_middleware.ChainUnaryServer(
				grpc_auth.UnaryServerInterceptor(utils.BtsAuthFunc),
				utils.UnaryInterceptor,
			),
		),
	)

	pb.RegisterEngineToBtsServer(s, &server{})
	grpcHealthServer := health.NewServer()
	healthpb.RegisterHealthServer(s, grpcHealthServer)
	reflection.Register(s)
	logger.Info("a2dbg Server listening on: ", listenIp)
	if err := s.Serve(lis); err != nil {
		logger.Fatalf("failed to serve: %v", err)
	}
}
