package server

import (
	"context"
	"net"
	"net/url"
	"strings"

	. "github.com/acompany-develop/QuickMPC/packages/server/manage_container/Log"

	"google.golang.org/grpc"
	"google.golang.org/grpc/health"
	"google.golang.org/grpc/health/grpc_health_v1"
)

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

func NewServer() *grpc.Server {
	s := grpc.NewServer(
		grpc.UnaryInterceptor(unaryInterceptor),
	)
	grpc_health_v1.RegisterHealthServer(s, health.NewServer())
	return s
}

func Listen(ip *url.URL) (net.Listener, string) {
	port := strings.Split(ip.Host, ":")[1]
	port = ":" + port
	lis, err := net.Listen("tcp", port)
	if err != nil {
		AppLogger.Fatalf("failed to listen: %v", err)
	}
	return lis, port
}
