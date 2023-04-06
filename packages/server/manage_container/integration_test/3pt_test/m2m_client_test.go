package integration_3pt_test

import (
	"fmt"
	"os"
	"testing"
	"time"

	m2db "github.com/acompany-develop/QuickMPC/packages/server/manage_container/client/manage_to_db"
	m2m "github.com/acompany-develop/QuickMPC/packages/server/manage_container/client/manage_to_manage_container"
	utils "github.com/acompany-develop/QuickMPC/packages/server/manage_container/utils"
	pb_types "github.com/acompany-develop/QuickMPC/proto/common_types"
)

func TestDeleteShares(t *testing.T) {
	dataID := "deleteShares"
	deleteId(t, dataID)
	// delete用のdataを送信
	dbclient := m2db.Client{}
	dbclient.InsertShares(dataID, []*pb_types.Schema{}, 1, "[]", "", 1)

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
	_, err = os.Stat(fmt.Sprintf("/db/share/%s", dataID))
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
		dbclient.InsertShares(dataID, []*pb_types.Schema{}, 0, "[]", "", 1)
		// PT1にdata送信の終了を通知
		m2m_client.Sync("insert")
		// PT1のdata削除リクエストの終了を待機
		m2m_client.Sync("delete")
		// 同期させてから削除されてるかチェック
		_, err = os.Stat(fmt.Sprintf("/db/share/%s", dataID))
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

func TestCreateStatusFile(t *testing.T) {
	/**
	* 各手順ごとにsyncで同期
	* 1. PT1がstatu_RECEIVED作成リクエストを送る
	* 2. PT2,3の作成されたか確認
	 */
	jobUUID := "createStatusFile"
	deleteId(t, jobUUID)
	config, err := utils.GetConfig()
	if err != nil {
		t.Fatal(err)
	}
	id := config.PartyID
	m2m_client := m2m.Client{}
	if id == 1 {
		// NOTE: PT1はPT2,3へstatu_RECEIVED作成リクエストを送る
		client := m2m.Client{}
		err := client.CreateStatusFile(jobUUID)
		if err != nil {
			t.Fatal(err)
		}
		// data削除の終了を通知
		m2m_client.Sync("createStatusFile")
	} else {
		// NOTE: PT2,3はstatu_RECEIVED作成リクエストを受け取る
		// PT1のstatu_RECEIVED作成リクエストの終了を待機
		m2m_client.Sync("createStatusFile")
		// 同期させてから作成されてるかチェック
		_, err = os.Stat(fmt.Sprintf("/db/result/%s/status_%s", jobUUID, pb_types.JobStatus(2).String()))
		if err != nil {
			t.Fatal("status_RECEIVED must be created")
		}
	}
	deleteId(t, jobUUID)
}

func TestDeleteStatusFile(t *testing.T) {
	/**
	* 各手順ごとにsyncで同期
	* 1. PT2,3がstatu_RECEIVEDを作成
	* 2. PT1が削除リクエストを送信
	* 3. PT2,3で削除されたか確認
	 */
	jobUUID := "deleteStatusFile"
	deleteId(t, jobUUID)

	config, err := utils.GetConfig()
	if err != nil {
		t.Fatal(err)
	}
	id := config.PartyID
	m2m_client := m2m.Client{}
	if id == 1 {
		// NOTE: PT1はPT2,3へstatu_RECEIVED削除リクエストを送る
		m2m_client.Sync("create")
		client := m2m.Client{}
		err := client.DeleteStatusFile(jobUUID)
		if err != nil {
			t.Fatal(err)
		}
		// 削除リクエスト完了の通知
		m2m_client.Sync("deleteStatusFile")
	} else {
		// NOTE: PT2,3はstatu_RECEIVED作成リクエストを受け取る
		// PT2,3はstatu_RECEIVEDを作成する
		path := fmt.Sprintf("/db/result/%s", jobUUID)
		os.Mkdir(path, 0777)
		fp, _ := os.Create(fmt.Sprintf("%s/status_%s", path, pb_types.JobStatus(2).String()))
		fp.Close()
		// 作成完了を通知
		m2m_client.Sync("create")
		// PT1からの削除リクエストの終了を待機
		m2m_client.Sync("deleteStatusFile")
		// 同期させてから削除されてるかチェック
		_, err = os.Stat(fmt.Sprintf("/db/result/%s", jobUUID))
		if err == nil {
			t.Fatal(fmt.Sprintf("%s directory must be deleted", jobUUID))
		}
	}
	deleteId(t, jobUUID)
}
