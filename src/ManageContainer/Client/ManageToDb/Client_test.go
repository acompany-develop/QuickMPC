package mng2db

import (
	"fmt"
	"os"
	"path/filepath"
	"regexp"
	"testing"
)

// DBを初期化する
func initialize() {
	isGitkeep := regexp.MustCompile(".*.gitkeep")
	files, _ := filepath.Glob("/Db/*/*")
	for _, f := range files {
		if !isGitkeep.MatchString(f) {
			os.RemoveAll(f)
		}
	}
}

/* InsertShares(string, []string, int32, string, string) error */
var defaultSchema = []string{"attr1", "attr2", "attr3"}

const defaultDataID = "m2db_test_dataid"
const defaultPieceID = 1
const defaultShares = "[[\"1\",\"2\",\"3\"],[\"4\",\"5\",\"6\"]]"
const defaultSentAt = ""

// シェアが保存されるかTest
func TestInsertSharesSuccess(t *testing.T) {
	initialize()

	client := Client{}
	err_insert := client.InsertShares(defaultDataID, defaultSchema, defaultPieceID, defaultShares, defaultSentAt)

	if err_insert != nil {
		t.Error("insert shares faild: " + err_insert.Error())
	}
	_, err_exist := os.Stat(fmt.Sprintf("/Db/share/%s", defaultDataID))
	if err_exist != nil {
		t.Error("insert shares faild: " + err_exist.Error())
	}

	initialize()
}

// 重複シェアを弾くかTest
func TestInsertSharesRejectDuplicateDataId(t *testing.T) {
	initialize()
	const sameDataID = defaultDataID

	client := Client{}
	client.InsertShares(defaultDataID, defaultSchema, defaultPieceID, defaultShares, defaultSentAt)
	err_insert := client.InsertShares(sameDataID, defaultSchema, defaultPieceID, defaultShares, defaultSentAt)

	if err_insert == nil {
		t.Error("insert duplicate shares must be failed, but success.")
	}

	initialize()
}

/* DeleteShares([]string) error */
// dataIDを指定して削除されるかTest
func TestDeleteSharesSuccess(t *testing.T) {
	initialize()

	os.Mkdir(fmt.Sprintf("/Db/share/%s", defaultDataID), 0777)
	var dataIds = []string{defaultDataID}

	client := Client{}
	client.DeleteShares(dataIds)

	_, err_exist := os.Stat(fmt.Sprintf("/Db/share/%s", defaultDataID))
	if err_exist == nil {
		t.Error(fmt.Sprintf("delete shares faild: '/Db/share/%s' must be deleted, but exist", defaultDataID))
	}
	initialize()
}

/* GetSchema(string) ([]string, error) */
// Schemaが取得できるかTest
func TestGetSchemaSuccess(t *testing.T) {
	initialize()

	os.Mkdir(fmt.Sprintf("/Db/share/%s", defaultDataID), 0777)
	// TODO: insert shareと同じことをするため保留

	client := Client{}
	client.GetSchema(defaultDataID)

	initialize()
}

/* GetComputationResult(string) ([]*ComputationResult, error) */
// 計算結果が取得できるかTest
func TestGetComputationResult(t *testing.T) {
	initialize()
	// TODO: insert shareと同じことをするため保留

	const jobId = ""

	client := Client{}
	client.GetComputationResult(jobId)

	initialize()
}

/* InsertModelParams(string, string, int32) error */
const defaultJobUUID = "m2db_test_jobuuid"
const defaultParams = "[\"1\",\"2\",\"3\"]"

// モデルパラメータが送れるかTest
func TestInsertModelParams(t *testing.T) {
	initialize()

	client := Client{}
	err_insert := client.InsertModelParams(defaultJobUUID, defaultParams, defaultPieceID)

	if err_insert != nil {
		t.Error("insert model params faild: " + err_insert.Error())
	}
	_, err_exist := os.Stat(fmt.Sprintf("/Db/share/%s", defaultJobUUID))
	if err_exist != nil {
		t.Error("insert model prarams faild: " + err_exist.Error())
	}

	initialize()
}

// 重複モデルパラメータを弾くかTest
func TestInsertModelParamsDuplicate(t *testing.T) {
	initialize()
	const sameJobUUID = defaultJobUUID

	client := Client{}
	client.InsertModelParams(defaultJobUUID, defaultParams, defaultPieceID)
	err_insert := client.InsertModelParams(sameJobUUID, defaultParams, defaultPieceID)

	if err_insert == nil {
		t.Error("insert duplicate model params must be failed, but success.")
	}

	initialize()
}

// XXX: オブジェクトストレージへの移行に備えて廃止予定
/* GetDataList() (string, error) */
