package e2bserver

import (
	"context"
	"encoding/base64"
	"fmt"
	"net"
	"os"
	"strconv"
	"strings"
	"time"

	"github.com/golang-jwt/jwt/v4"
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

	triples, err := tg.GetTriples(ctx, in.GetJobId(), partyId, in.GetAmount(), in.GetTripleType())
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

func btsAuthFunc(ctx context.Context) (context.Context, error) {
	tokenString, err := grpc_auth.AuthFromMD(ctx, "bearer")
	if err != nil {
		return nil, status.Errorf(
			codes.Unauthenticated,
			"could not read auth token: %v",
			err,
		)
	}

	claims, err := authJWT(tokenString)
	if err != nil {
		return nil, err
	}

	return context.WithValue(ctx, "claims", claims), nil
}

func getListenPort() (string, error) {
	port, ok := os.LookupEnv("PORT")
	if !ok {
		return "", status.Error(codes.Internal, "port is not provided")
	}
	return port, nil
}

func getSecret() ([]byte, error) {
	raw, ok := os.LookupEnv("JWT_SECRET_KEY")
	if !ok {
		return nil, status.Error(codes.Internal, "jwt auth key is not provided")
	}
	secret, err := base64.StdEncoding.DecodeString(raw)
	if err != nil {
		return nil, err
	}
	return secret, nil
}

func authJWT(tokenString string) (*jwt_types.Claim, error) {
	jwtSecret, err := getSecret()
	if err != nil {
		return nil, err
	}

	token, err := jwt.ParseWithClaims(tokenString, &jwt_types.Claim{}, func(token *jwt.Token) (interface{}, error) {
		// alg を確認するのを忘れない
		if signingMethod, ok := token.Method.(*jwt.SigningMethodHMAC); !ok || signingMethod.Alg() != "HS256" {
			return nil, fmt.Errorf("unexpected signing method: %v", token.Header["alg"])
		}
		return []byte(jwtSecret), nil
	})

	if err != nil {
		return nil, status.Errorf(codes.Unauthenticated, "invalid auth token: %v", err)
	}

	if !token.Valid {
		return nil, status.Errorf(codes.Unauthenticated, "invalid auth token: %v", err)
	}

	claims, ok := token.Claims.(*jwt_types.Claim)
	if !ok {
		return nil, status.Error(codes.Internal, "failed claims type assertions")
	}

	return claims, nil
}

// requestを受け取った際の共通処理
func unaryInterceptor(ctx context.Context, req interface{}, info *grpc.UnaryServerInfo, handler grpc.UnaryHandler) (interface{}, error) {
	// 定期実行されるhealth checkでlogは必要ないため即時return
	if info.FullMethod == "/grpc.health.v1.Health/Check" {
		return handler(ctx, req)
	}

	logger.Infof("received: %s", info.FullMethod)

	// TODO: read claims, and use these party information
	claims, ok := ctx.Value("claims").(*jwt_types.Claim)
	if ok {
		logger.Infof("claims: %v\n", claims)
	}

	// 処理を実行する
	res, err := handler(ctx, req)

	// エラー時にログとしてrequest，responseを出力する
	if err != nil {
		logger.Errorf("request: {%v}\tresponse: {%v}\n", req, res)
	}

	logger.Infof("send: %s", info.FullMethod)
	return res, err
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
				grpc_auth.UnaryServerInterceptor(btsAuthFunc),
				unaryInterceptor,
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
