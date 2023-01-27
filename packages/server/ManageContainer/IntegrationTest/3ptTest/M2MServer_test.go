package integration_3pt_test

import (
	"context"
	"fmt"
	"os"
	"testing"

	m2m "github.com/acompany-develop/QuickMPC/src/ManageContainer/Client/ManageToManageContainer"
	utils "github.com/acompany-develop/QuickMPC/src/ManageContainer/Utils"
	pb "github.com/acompany-develop/QuickMPC/src/proto/LibcToManageContainer"
	"google.golang.org/grpc"
)

// 自分へのconnecterを得る
func connect() (*grpc.ClientConn, error) {
	config, err := utils.GetConfig()
	if err != nil {
		return nil, err
	}
	IP := config.Containers.Manage
	conn, err := grpc.Dial(IP.Host, grpc.WithInsecure())
	if err != nil {
		return nil, fmt.Errorf("did not connect: %v", err)
	}
	return conn, nil
}

func TestSendShare(t *testing.T) {
	dataID := "sendShare"
	deleteId(t, dataID)
	conn, err := connect()
	if err != nil {
		t.Fatal(err)
	}
	defer conn.Close()

	req := &pb.SendSharesRequest{
		DataId:  dataID,
		Shares:  "[]",
		Schema:  []string{},
		PieceId: 0,
		SentAt:  "",
		Token:   "token_demo",
	}

	config, err := utils.GetConfig()
	if err != nil {
		t.Fatal(err)
	}

	m2m_client := m2m.Client{}
	m2m_client.Sync("send_start")
	client := pb.NewLibcToManageClient(conn)
	if config.PartyID == 1 {
		// NOTE: PT1 はShare送信を失敗させる
		req.Shares = "not json string"
		_, err := client.SendShares(context.Background(), req)
		if err == nil {
			t.Fatal(err)
		}
	} else {
		// NOTE: PT2,3 はShare送信を成功した後PT1から削除リクエストを受け取る
		_, err := client.SendShares(context.Background(), req)
		if err != nil {
			t.Fatal(err)
		}
	}

	// 削除リクエストにより削除されるのを待機
	m2m_client.Sync("send_end")

	_, err = os.Stat(fmt.Sprintf("/Db/share/%s", dataID))
	if err == nil {
		t.Fatal("dataID must be deleted")
	}

	deleteId(t, dataID)
}
