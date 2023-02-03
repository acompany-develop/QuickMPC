/*
 * Test用サーバーを構築するためのコード
 * bufconnによって他のコンテナを立てず接続Testができる
 */
package utils

import (
	"context"
	"net"

	. "github.com/acompany-develop/QuickMPC/packages/server/manage_container/Log"
	"google.golang.org/grpc"
	"google.golang.org/grpc/health"
	"google.golang.org/grpc/health/grpc_health_v1"
	"google.golang.org/grpc/test/bufconn"
)

const bufSize = 1024 * 1024

type TestServer struct {
	server *grpc.Server
	lis    *bufconn.Listener
}

func (s *TestServer) GetServer() *grpc.Server {
	s.server = grpc.NewServer()
	grpc_health_v1.RegisterHealthServer(s.server, health.NewServer())
	return s.server
}

func (s *TestServer) Serve() {
	s.lis = bufconn.Listen(bufSize)
	go func() {
		if err := s.server.Serve(s.lis); err != nil {
			AppLogger.Fatal(err)
		}
	}()
}

func (s *TestServer) bufDialer(ctx context.Context, address string) (net.Conn, error) {
	return s.lis.Dial()
}

func (s *TestServer) GetConn() *grpc.ClientConn {
	ctx := context.Background()
	conn, err := grpc.DialContext(ctx, "bufnet", grpc.WithContextDialer(s.bufDialer), grpc.WithInsecure())
	if err != nil {
	}
	return conn
}
