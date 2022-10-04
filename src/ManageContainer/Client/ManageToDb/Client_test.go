package mng2db

import (
	"fmt"
	"io/ioutil"
	"os"
	"path/filepath"
	"reflect"
	"regexp"
	"sync"
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

var defaultSchema = []string{"attr1", "attr2", "attr3"}

const defaultDataID = "m2db_test_dataid"
const defaultPieceID = 1
const defaultShares = "[[\"1\",\"2\",\"3\"],[\"4\",\"5\",\"6\"]]"
const defaultSentAt = ""
const defaultJobUUID = "m2db_test_jobuuid"
const defaultParams = "[\"1\",\"2\",\"3\"]"
const defaultResult = "[\"1\",\"2\",\"3\"]"

/* InsertShares(string, []string, int32, string, string) error */

// シェアが保存されるかTest
func TestInsertSharesSuccess(t *testing.T) {
	initialize()

	client := Client{}
	err_insert := client.InsertShares(defaultDataID, defaultSchema, defaultPieceID, defaultShares, defaultSentAt)

	if err_insert != nil {
		t.Error("insert shares faild: " + err_insert.Error())
	}
	_, err_exist := os.Stat(fmt.Sprintf("/Db/share/%s/%d", defaultDataID, defaultPieceID))
	if err_exist != nil {
		t.Error("insert shares faild: " + err_exist.Error())
	}

	initialize()
}

// 重複シェアを弾くかTest
func TestInsertSharesRejectDuplicate(t *testing.T) {
	initialize()

	client := Client{}
	client.InsertShares(defaultDataID, defaultSchema, defaultPieceID, defaultShares, defaultSentAt)
	err_insert := client.InsertShares(defaultDataID, defaultSchema, defaultPieceID, defaultShares, defaultSentAt)

	if err_insert == nil {
		t.Error("insert duplicate shares must be failed, but success.")
	}

	initialize()
}

// pieceが同時に送信されて保存されるかTest
func TestInsertSharesParallelSuccess(t *testing.T) {
	initialize()

	client := Client{}
	wg := sync.WaitGroup{}
	for i := 1; i <= 100; i++ {
		wg.Add(1)
		go func(pieceId int32) {
			err_insert := client.InsertShares(defaultDataID, defaultSchema, pieceId, defaultShares, defaultSentAt)
			if err_insert != nil {
				t.Error("insert shares faild: " + err_insert.Error())
			}
			_, err_exist := os.Stat(fmt.Sprintf("/Db/share/%s/%d", defaultDataID, pieceId))
			if err_exist != nil {
				t.Error("insert shares faild: " + err_exist.Error())
			}
			defer wg.Done()
		}(int32(i))
	}
	wg.Wait()

	initialize()
}

// 同じShareが同時に送信されてエラーが出るかTest
func TestInsertSharesParallelRejectDuplicate(t *testing.T) {
	initialize()

	client := Client{}
	client.InsertShares(defaultDataID, defaultSchema, defaultPieceID, defaultShares, defaultSentAt)
	wg := sync.WaitGroup{}
	for i := 1; i <= 100; i++ {
		wg.Add(1)
		go func() {
			err_insert := client.InsertShares(defaultDataID, defaultSchema, defaultPieceID, defaultShares, defaultSentAt)
			if err_insert == nil {
				t.Error("insert duplicate shares must be failed, but success.")
			}
			defer wg.Done()
		}()
	}
	wg.Wait()

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
	data := "{\"meta\":{\"schema\":[\"attr1\",\"attr2\",\"attr3\"]}}"
	ioutil.WriteFile(fmt.Sprintf("/Db/share/%s/%d", defaultDataID, defaultPieceID), []byte(data), 0666)

	client := Client{}
	schema, err := client.GetSchema(defaultDataID)

	if err != nil {
		t.Error("get schema faild: " + err.Error())
	}
	cor := []string{"attr1", "attr2", "attr3"}
	if !reflect.DeepEqual(schema, cor) {
		t.Errorf("get schema faild: schema must be %v, but value is %v", cor, schema)
	}

	initialize()
}

// 保存されていないSchemaでエラーがでるかTest
func TestGetSchemaFailedEmptyID(t *testing.T) {
	initialize()

	client := Client{}
	_, err := client.GetSchema(defaultDataID)

	if err == nil {
		t.Error("get schema must be faild: data is not registered.")
	}

	initialize()
}

/* GetComputationResult(string) ([]*ComputationResult, error) */
// 計算結果が取得できるかTest
func TestGetComputationResultSuccess(t *testing.T) {
	initialize()

	os.Mkdir(fmt.Sprintf("/Db/result/%s", defaultJobUUID), 0777)
	data := "{\"id\":\"\",\"job_uuid\":\"m2db_test_jobuuid\",\"status\":1,\"result\":\"[\\\"1\\\",\\\"2\\\",\\\"3\\\"]\",\"meta\":{\"piece_id\":1}}"
	ioutil.WriteFile(fmt.Sprintf("/Db/result/%s/%d", defaultJobUUID, defaultPieceID), []byte(data), 0666)
	os.Create(fmt.Sprintf("/Db/result/%s/completed", defaultJobUUID))

	client := Client{}
	result, err := client.GetComputationResult(defaultJobUUID)

	if err != nil {
		t.Error("get computation result faild: " + err.Error())
	}
	if result[0].JobUUID != defaultJobUUID {
		t.Error(fmt.Sprintf("get computation result faild: JobUUID must be %s, but value is %s", defaultJobUUID, result[0].JobUUID))
	}
	if result[0].Result != defaultResult {
		t.Error(fmt.Sprintf("get computation result faild: Result must be %s, but value is %s", defaultResult, result[0].Result))
	}
	if result[0].Meta.PieceID != defaultPieceID {
		t.Error(fmt.Sprintf("get computation result faild: JobUUID must be %d, but value is %d", defaultPieceID, result[0].Meta.PieceID))
	}
	if result[0].Status != 1 {
		t.Error(fmt.Sprintf("get computation result faild: JobUUID must be %d, but value is %d", 1, result[0].Status))
	}

	initialize()
}

// 計算結果が存在しない場合にエラーがでるかTest
func TestGetComputationResultFailedEmptyResult(t *testing.T) {
	initialize()

	client := Client{}
	_, err := client.GetComputationResult(defaultJobUUID)

	if err == nil {
		t.Error("get computation result must be faild: result is not registered.")
	}

	initialize()
}

// completedファイルが存在しない場合にエラーがでるかTest
func TestGetComputationResultFailedEmptyComplated(t *testing.T) {
	initialize()

	os.Mkdir(fmt.Sprintf("/Db/result/%s", defaultJobUUID), 0777)
	data := "{\"id\":\"\",\"job_uuid\":\"m2db_test_jobuuid\",\"status\":1,\"result\":\"[\\\"1\\\",\\\"2\\\",\\\"3\\\"]\",\"meta\":{\"piece_id\":1}}"
	ioutil.WriteFile(fmt.Sprintf("/Db/result/%s/%d", defaultJobUUID, defaultPieceID), []byte(data), 0666)

	client := Client{}
	_, err := client.GetComputationResult(defaultJobUUID)

	if err == nil {
		t.Error("get computation result must be faild: computation is running(complated file is not found).")
	}

	initialize()
}

/* InsertModelParams(string, string, int32) error */
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
