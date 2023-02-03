package integration_3pt_test

import (
	"fmt"
	"os"
	"testing"
	"time"

	m2db "github.com/acompany-develop/QuickMPC/packages/server/manage_container/Client/ManageToDb"
	m2m "github.com/acompany-develop/QuickMPC/packages/server/manage_container/Client/ManageToManageContainer"
	utils "github.com/acompany-develop/QuickMPC/packages/server/manage_container/Utils"
)

func TestDeleteShares(t *testing.T) {
	dataID := "deleteShares"
	deleteId(t, dataID)
	// delete用のdataを送信
	dbclient := m2db.Client{}
	dbclient.InsertShares(dataID, []string{}, 1, "[]", "", 1)

	// insertを同期してから他のPTへdata削除リクエスト
	m2m_client := m2m.Client{}
	m2m_client.Sync("insert")
	client := m2m.Client{}
	err := client.DeleteShares(dataID)
	if err != nil {
		t.Fatal(err)
	}

	// 同期させてから削除されてるかチェック
	m2m_client.Sync("delete_all")
	_, err = os.Stat(fmt.Sprintf("/Db/share/%s", dataID))
	if err == nil {
		t.Fatal("dataID must be deleted")
	}
}

func TestDeleteShares1PT(t *testing.T) {
	/**
	* 各手順ごとにsyncで同期
	* 1. PT2,3がデータを保存
	* 2. PT1がデータ削除リクエストを送信
	* 3. PT2,3のデータが削除されたか確認
	 */
	dataID := "deleteShares1PT"
	deleteId(t, dataID)
	config, err := utils.GetConfig()
	if err != nil {
		t.Fatal(err)
	}
	id := config.PartyID
	m2m_client := m2m.Client{}
	if id == 1 {
		// NOTE: PT1はPT2,3へ削除リクエストを送る
		// PT2,3がデータ挿入するのを待機
		m2m_client.Sync("insert")
		// data削除リクエスト
		client := m2m.Client{}
		err := client.DeleteShares(dataID)
		if err != nil {
			t.Fatal(err)
		}
		// data削除の終了を通知
		m2m_client.Sync("delete")
	} else {
		// NOTE: PT2,3は削除リクエストを受け取る
		// delete用のdataを送信
		dbclient := m2db.Client{}
		dbclient.InsertShares(dataID, []string{}, 0, "[]", "", 1)
		// PT1にdata送信の終了を通知
		m2m_client.Sync("insert")
		// PT1のdata削除リクエストの終了を待機
		m2m_client.Sync("delete")
		// 同期させてから削除されてるかチェック
		_, err = os.Stat(fmt.Sprintf("/Db/share/%s", dataID))
		if err == nil {
			t.Fatal("dataID must be deleted")
		}
	}
	deleteId(t, dataID)
}

func TestSync(t *testing.T) {
	timeout := time.After(10 * time.Second)
	done := make(chan bool)

	go func() {
		client := m2m.Client{}
		for n := 1; n < 100; n++ {
			id := fmt.Sprintf("id%d", n)
			client.Sync(id)
		}
		done <- true
	}()

	select {
	case <-timeout:
		t.Fatal("Sync method never ends. Maybe waiting all the time.")
	case <-done:
	}
}
