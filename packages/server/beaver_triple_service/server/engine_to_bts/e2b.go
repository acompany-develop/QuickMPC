package e2bserver

import (
	"context"
	"fmt"
	"net"
	"os"
	"strconv"
	"strings"
	"time"

	grpc_middleware "github.com/grpc-ecosystem/go-grpc-middleware"
	grpc_auth "github.com/grpc-ecosystem/go-grpc-middleware/auth"
	"google.golang.org/grpc"
	"google.golang.org/grpc/codes"
	"google.golang.org/grpc/health"
	healthpb "google.golang.org/grpc/health/grpc_health_v1"
	"google.golang.org/grpc/keepalive"
	"google.golang.org/grpc/metadata"
	"google.golang.org/grpc/peer"
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

// モック時に置き換わる関数
var GetPartyIdFromIp = func(ctx context.Context, reqIpAddrAndPort string) (uint32, error) {
	arr := strings.Split(reqIpAddrAndPort, ":")
	if len(arr) != 2 {
		errText := fmt.Sprintf("requestのIpAddessの形式が異常: %s", reqIpAddrAndPort)
		logger.Error(errText)
		return 0, fmt.Errorf(errText)
	}
	reqIpAddr, _ := arr[0], arr[1]

	var partyId uint32
	claims, _ := ctx.Value("claims").(*jwt_types.Claim)
	for _, party := range claims.PartyInfo {
		if reqIpAddr == party.Address {
			partyId = party.Id
			break
		}
	}
	if partyId == 0 {
		errText := fmt.Sprintf("PartyList[%s, %s, %s]に存在しないIPからのリクエスト: %s", claims.PartyInfo[0].Address, claims.PartyInfo[1].Address, claims.PartyInfo[2].Address, reqIpAddr)
		logger.Error(errText)
		return 0, fmt.Errorf(errText)
	}

	return partyId, nil
}

// ClientのIPアドレスを取得する関数
func GetReqIpAddrAndPort(ctx context.Context) string {
	var reqIpAddrAndPort string
	claims, _ := ctx.Value("claims").(*jwt_types.Claim)

	if claims.WithEnvoy {
		md, _ := metadata.FromIncomingContext(ctx)
		port := strconv.FormatUint(uint64(claims.Port), 10)
		reqIpAddrAndPort = fmt.Sprintf("%s:%s",md["x-forwarded-for"][0], port)
	} else {
		p, _ := peer.FromContext(ctx)
		reqIpAddrAndPort = p.Addr.String()
	}

	return reqIpAddrAndPort
}

func (s *server) GetTriples(ctx context.Context, in *pb.GetTriplesRequest) (*pb.GetTriplesResponse, error) {
	// ClientのIPアドレスを取得
	reqIpAddrAndPort := GetReqIpAddrAndPort(ctx)

	partyId, err := GetPartyIdFromIp(ctx, reqIpAddrAndPort)
	if err != nil {
		return nil, err
	}
	logger.Infof("Ip %s, jobId: %d, partyId: %d Type: %v\n", reqIpAddrAndPort, in.GetJobId(), partyId, in.GetTripleType())

	claims, ok := ctx.Value("claims").(*jwt_types.Claim)
	if !ok {
		return nil, status.Error(codes.Internal, "failed claims type assertions")
	}

	triples, err := tg.GetTriples(claims, in.GetJobId(), partyId, in.GetAmount(), in.GetTripleType())
	if err != nil {
		return nil, err
	}

	return &pb.GetTriplesResponse{
		Triples: triples,
	}, nil
}

func (s *server) InitTripleStore(ctx context.Context, in *emptypb.Empty) (*emptypb.Empty, error) {
	// ClientのIPアドレスを取得
	reqIpAddrAndPort := GetReqIpAddrAndPort(ctx)

	partyId, err := GetPartyIdFromIp(ctx, reqIpAddrAndPort)
	if err != nil {
		return nil, err
	}
	logger.Infof("Ip %s, partyId: %d \n", reqIpAddrAndPort, partyId)

	err = tg.InitTripleStore()
	if err != nil {
		return nil, err
	}

	return &emptypb.Empty{},err
}

func (s *server) DeleteJobIdTriple(ctx context.Context, in *pb.DeleteJobIdTripleRequest) (*emptypb.Empty, error) {
	// ClientのIPアドレスを取得
	reqIpAddrAndPort := GetReqIpAddrAndPort(ctx)

	partyId, err := GetPartyIdFromIp(ctx, reqIpAddrAndPort)
	if err != nil {
		return nil, err
	}
	logger.Infof("Ip %s, jobId: %d, partyId: %d\n", reqIpAddrAndPort, in.GetJobId(), partyId)

	err = tg.DeleteJobIdTriple(in.GetJobId())
	if err != nil {
		return nil, err
	}

	return &emptypb.Empty{},err
}

func getListenPort() (string, error) {
	port, ok := os.LookupEnv("PORT")
	if !ok {
		return "", status.Error(codes.Internal, "port is not provided")
	}
	return port, nil
}

func RunServer() {
	listenPort, err := getListenPort()
	if err != nil{
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
