package integration_3pt_test

import (
	"fmt"
	"os"
	"testing"
	"time"

	m2m "github.com/acompany-develop/QuickMPC/packages/server/manage_container/client/manage_to_manage_container"
	l2mserver "github.com/acompany-develop/QuickMPC/packages/server/manage_container/server/libc_to_manage_container"
	m2mserver "github.com/acompany-develop/QuickMPC/packages/server/manage_container/server/manage_to_manage_container"
)

func TestMain(m *testing.M) {

	// サーバを起動する
	go m2mserver.RunServer()
	go l2mserver.RunServer()
	time.Sleep(time.Second)

	// Test実行タイミングを全てのPTで同期させる
	client := m2m.Client{}
	client.Sync("start")

	// Testを実行
	code := m.Run()

	// 他のPTがサーバが使うのを少し待機
	// (自分だけTestを終えて早々にサーバを閉じることがあるため)
	time.Sleep(3 * time.Second)

	os.Exit(code)
}

func deleteId(t *testing.T, id string) {
	os.RemoveAll(fmt.Sprintf("/db/share/%s", id))
	os.RemoveAll(fmt.Sprintf("/db/result/%s", id))
}
