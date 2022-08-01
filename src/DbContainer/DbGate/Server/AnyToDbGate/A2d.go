package a2dbgserver

import (
	"context"
	"net"
	"strings"

	"google.golang.org/genproto/googleapis/rpc/errdetails"
	"google.golang.org/grpc"
	"google.golang.org/grpc/codes"
	"google.golang.org/grpc/health"
	"google.golang.org/grpc/health/grpc_health_v1"
	"google.golang.org/grpc/reflection"
	"google.golang.org/grpc/status"

	dbg2db "github.com/acompany-develop/QuickMPC/src/DbContainer/DbGate/Client/DbGateToDb"
	. "github.com/acompany-develop/QuickMPC/src/DbContainer/DbGate/Log"
	utils "github.com/acompany-develop/QuickMPC/src/DbContainer/DbGate/Utils"
	pb "github.com/acompany-develop/QuickMPC/src/Proto/AnyToDbGate"
)

type server struct {
	pb.UnimplementedAnyToDbGateServer
}

func (s *server) ExecuteQuery(in *pb.ExecuteQueryRequest, stream pb.AnyToDbGate_ExecuteQueryServer) error {

	opt := dbg2db.QueryOpts{
		Host:  in.GetHost(),
		Query: in.GetQuery(),
	}

	// DBコンテナにクエリを投げる
	result, err := dbg2db.ExecuteQuery(&opt)

	if err != nil {
		AppLogger.Error(err)
		st := status.New(codes.Internal, "予期せぬエラーによりDBクエリ失敗")
		details := &errdetails.ErrorInfo{
			Reason: "DB_ERROR",
			Domain: "dbcontainer_dbgate",
		}
		st, _ = st.WithDetails(details)
		return st.Err()
	}
	// https://text.baldanders.info/golang/string-and-rune/
	runes := []rune(result)
	var piece_id int32 = 0
	piece_size := 1000000
	for i := 0; i < len(runes); i += piece_size {
		var piece string
		if i+piece_size < len(runes) {
			piece = string(runes[i:(i + piece_size)])
		} else {
			piece = string(runes[i:])
		}
		if err := stream.Send(&pb.ExecuteQueryResponse{
			Result:  piece,
			PieceId: piece_id,
		}); err != nil {
			return err
		}
		piece_id = piece_id + 1
	}
	return nil
}

// requestを受け取った際の共通処理
func unaryInterceptor(ctx context.Context, req interface{}, info *grpc.UnaryServerInfo, handler grpc.UnaryHandler) (interface{}, error) {
	// 処理を実行する
	res, err := handler(ctx, req)

	// エラー時にログとしてrequest，responseを出力する
	if err != nil {
		AppLogger.Errorf("request: {%v}\tresponse: {%v}\n", req, res)
	}
	return res, err
}

func RunServer() {
	config, err := utils.GetConfig()
	if err != nil {
		AppLogger.Fatalf("Config読み込みエラー: %v", err)
	}

	u := config.Containers.Dbgate
	port := strings.Split(u.Host, ":")[1]
	port = ":" + port
	lis, err := net.Listen("tcp", port)
	if err != nil {
		AppLogger.Fatalf("failed to listen: %v", err)
	}

	s := grpc.NewServer(
		grpc.UnaryInterceptor(unaryInterceptor),
	)
	reflection.Register(s)

	pb.RegisterAnyToDbGateServer(s, &server{})
	grpc_health_v1.RegisterHealthServer(s, health.NewServer())

	AppLogger.Info("a2dbg Server listening on", port)
	if err := s.Serve(lis); err != nil {
		AppLogger.Fatalf("failed to serve: %v", err)
	}
	dbg2db.CloseCluster()
}
