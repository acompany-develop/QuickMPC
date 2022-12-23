package mng2mng

import (
	"context"
	"crypto/tls"
	"fmt"
	"time"

	helper "github.com/acompany-develop/QuickMPC/src/ManageContainer/Client/Helper"
	m2mserver "github.com/acompany-develop/QuickMPC/src/ManageContainer/Server/ManageToManageContainer"
	utils "github.com/acompany-develop/QuickMPC/src/ManageContainer/Utils"
	pb "github.com/acompany-develop/QuickMPC/src/Proto/ManageToManageContainer"
	"google.golang.org/grpc"
	"google.golang.org/grpc/connectivity"
	"google.golang.org/grpc/credentials"
	"google.golang.org/grpc/credentials/insecure"
)

type Client struct{}

type M2MClient interface {
	DeleteShares(string) error
	Sync(string) error
}

// 自分以外のMCへのconnecterを得る
func connect() ([]*grpc.ClientConn, error) {
	config, err := utils.GetConfig()
	connList := []*grpc.ClientConn{}
	ID := config.PartyID
	for _, party := range config.Containers.PartyList {
		// 自分のIDの場合はスキップ
		if party.PartyID == ID {
			continue
		}
		McIP := party.IpAddress

		if err != nil {
			return nil, err
		}
		var conn *grpc.ClientConn
		if McIP.Scheme == "http" {
			conn, err = grpc.Dial(McIP.Host, grpc.WithTransportCredentials(insecure.NewCredentials()))
		} else if McIP.Scheme == "https" {
			config := &tls.Config{}

			conn, err = grpc.Dial(McIP.Host, grpc.WithTransportCredentials(credentials.NewTLS(config)))
		} else {
			return nil, fmt.Errorf("Not supported scheme: %v", McIP.Scheme)
		}
		if err != nil {
			return nil, fmt.Errorf("did not connect: %v", err)
		}
		connList = append(connList, conn)
	}
	return connList, nil
}

func reconnect(conn *grpc.ClientConn) bool {
	// 20秒間だけ再接続を試みる
	ctx, cancel := context.WithTimeout(context.Background(), 20*time.Second)
	defer cancel()
	conn.WaitForStateChange(ctx, conn.GetState())
	return conn.GetState() == connectivity.Idle
}

// 自分以外のMCにシェア削除リクエストを送信する
func (c Client) DeleteShares(dataID string) error {
	connList, err := connect()
	if err != nil {
		return err
	}
	for _, conn := range connList {
		defer conn.Close()
		err = c.deleteShares(conn, dataID)
		if err != nil {
			return err
		}
	}
	return nil
}

// (conn)にシェア削除リクエストを送信する
func (c Client) deleteShares(conn *grpc.ClientConn, dataID string) error {
	mcTomcClient := pb.NewManageToManageClient(conn)

	deleteSharesRequest := &pb.DeleteSharesRequest{DataId: dataID}
	rm := helper.RetryManager{}
	for {
		_, err := mcTomcClient.DeleteShares(context.TODO(), deleteSharesRequest)
		retry, _ := rm.Retry(err)
		if !retry {
			return err
		}
	}
}

// 自分以外のMCにSyncリクエストを送信する
func (c Client) Sync(syncID string) error {
	connList, err := connect()
	if err != nil {
		return err
	}

	for _, conn := range connList {
		defer conn.Close()
		err = c.sync(conn, syncID)
		if err != nil {
			return err
		}
	}

	// 他のMCからリクエストが来るのをタイムアウト付きで待機
	timeout := time.After(50 * time.Second)
	done := make(chan struct{})
	go func() {
		m2mserver.Wait(syncID, func(cnt int) bool { return cnt < len(connList) })
		done <- struct{}{}
	}()
	select {
	case <-timeout:
		return fmt.Errorf("Sync response is not returned ERROR!")
	case <-done:
		return nil
	}
}

// (conn)にシェア削除リクエストを送信する
func (c Client) sync(conn *grpc.ClientConn, syncID string) error {
	mcTomcClient := pb.NewManageToManageClient(conn)
	syncRequest := &pb.SyncRequest{SyncId: syncID}
	rm := helper.RetryManager{}
	for {
		_, err := mcTomcClient.Sync(context.TODO(), syncRequest)
		retry, _ := rm.Retry(err)
		if !retry {
			return err
		}
	}
}
