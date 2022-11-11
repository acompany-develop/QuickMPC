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
	"math"

	pb_types "github.com/acompany-develop/QuickMPC/src/Proto/common_types"
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
const defaultPieceID = 0
const defaultShares = `[["1","2","3"],["4","5","6"]]`
const defaultSentAt = ""
const defaultMatchingColumn = 1
const defaultJobUUID = "m2db_test_jobuuid"
const defaultParams = `["1","2","3"]`
const defaultResult = `["1","2","3"]`

/* InsertShares(string, []string, int32, string, string) error */
// シェアが保存されるかTest
func TestInsertSharesSuccess(t *testing.T) {
	initialize()

	client := Client{}
	errInsert := client.InsertShares(defaultDataID, defaultSchema, defaultPieceID, defaultShares, defaultSentAt, defaultMatchingColumn)

	if errInsert != nil {
		t.Error("insert shares failed: " + errInsert.Error())
	}
	_, errExist := os.Stat(fmt.Sprintf("/Db/share/%s/%d", defaultDataID, defaultPieceID))
	if errExist != nil {
		t.Error("insert shares failed: " + errExist.Error())
	}

	initialize()
}

// 重複シェアを弾くかTest
func TestInsertSharesRejectDuplicate(t *testing.T) {
	initialize()

	client := Client{}
	client.InsertShares(defaultDataID, defaultSchema, defaultPieceID, defaultShares, defaultSentAt, defaultMatchingColumn)
	errInsert := client.InsertShares(defaultDataID, defaultSchema, defaultPieceID, defaultShares, defaultSentAt, defaultMatchingColumn)

	if errInsert == nil {
		t.Error("insert duplicate shares must be failed, but success.")
	}

	initialize()
}

// pieceが同時に送信されて保存されるかTest
func TestInsertSharesParallelSuccess(t *testing.T) {
	initialize()

	client := Client{}
	wg := sync.WaitGroup{}
	for i := 0; i < 100; i++ {
		wg.Add(1)
		go func(pieceId int32) {
			errInsert := client.InsertShares(defaultDataID, defaultSchema, pieceId, defaultShares, defaultSentAt, defaultMatchingColumn)
			if errInsert != nil {
				t.Error("insert shares failed: " + errInsert.Error())
			}
			_, errExist := os.Stat(fmt.Sprintf("/Db/share/%s/%d", defaultDataID, pieceId))
			if errExist != nil {
				t.Error("insert shares failed: " + errExist.Error())
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
	client.InsertShares(defaultDataID, defaultSchema, defaultPieceID, defaultShares, defaultSentAt, defaultMatchingColumn)
	wg := sync.WaitGroup{}
	for i := 0; i < 100; i++ {
		wg.Add(1)
		go func() {
			errInsert := client.InsertShares(defaultDataID, defaultSchema, defaultPieceID, defaultShares, defaultSentAt, defaultMatchingColumn)
			if errInsert == nil {
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

	_, errExist := os.Stat(fmt.Sprintf("/Db/share/%s", defaultDataID))
	if errExist == nil {
		t.Error(fmt.Sprintf("delete shares failed: '/Db/share/%s' must be deleted, but exist", defaultDataID))
	}
	initialize()
}

/* GetSchema(string) ([]string, error) */
// Schemaが取得できるかTest
func TestGetSchemaSuccess(t *testing.T) {
	initialize()

	os.Mkdir(fmt.Sprintf("/Db/share/%s", defaultDataID), 0777)
	data := `{"meta":{"schema":["attr1","attr2","attr3"]}}`
	ioutil.WriteFile(fmt.Sprintf("/Db/share/%s/%d", defaultDataID, defaultPieceID), []byte(data), 0666)

	client := Client{}
	schema, err := client.GetSchema(defaultDataID)

	if err != nil {
		t.Error("get schema failed: " + err.Error())
	}
	cor := []string{"attr1", "attr2", "attr3"}
	if !reflect.DeepEqual(schema, cor) {
		t.Errorf("get schema failed: schema must be %v, but value is %v", cor, schema)
	}

	initialize()
}

// 保存されていないSchemaでエラーがでるかTest
func TestGetSchemaFailedEmptyID(t *testing.T) {
	initialize()

	client := Client{}
	_, err := client.GetSchema(defaultDataID)

	if err == nil {
		t.Error("get schema must be failed: data is not registered.")
	}

	initialize()
}

/* GetComputationResult(string) ([]*ComputationResult, error) */
// 計算結果が取得できるかTest
func TestGetComputationResultSuccess(t *testing.T) {
	initialize()

	os.Mkdir(fmt.Sprintf("/Db/result/%s", defaultJobUUID), 0777)
	data := `{"id":"","job_uuid":"m2db_test_jobuuid","result":"[\"1\",\"2\",\"3\"]","meta":{"piece_id":0}}`
	ioutil.WriteFile(fmt.Sprintf("/Db/result/%s/%d", defaultJobUUID, defaultPieceID), []byte(data), 0666)
	os.Create(fmt.Sprintf("/Db/result/%s/completed", defaultJobUUID))
	os.Create(fmt.Sprintf("/Db/result/%s/status_COMPLETED", defaultJobUUID))

	client := Client{}
	result, err := client.GetComputationResult(defaultJobUUID)

	if err != nil {
		t.Error("get computation result failed: " + err.Error())
	}
	if result[0].JobUUID != defaultJobUUID {
		t.Error(fmt.Sprintf("get computation result failed: JobUUID must be %s, but value is %s", defaultJobUUID, result[0].JobUUID))
	}
	if result[0].Result != defaultResult {
		t.Error(fmt.Sprintf("get computation result failed: Result must be %s, but value is %s", defaultResult, result[0].Result))
	}
	if result[0].Meta.PieceID != defaultPieceID {
		t.Error(fmt.Sprintf("get computation result failed: PieceID must be %d, but value is %d", defaultPieceID, result[0].Meta.PieceID))
	}
	if result[0].Status != int32(pb_types.JobStatus_COMPLETED) {
		t.Error(fmt.Sprintf("get computation result failed: Status must be %d, but value is %d", pb_types.JobStatus_COMPLETED, result[0].Status))
	}

	initialize()
}

// 計算結果が存在しない場合にエラーがでるかTest
func TestGetComputationResultFailedEmptyResult(t *testing.T) {
	initialize()

	client := Client{}
	_, err := client.GetComputationResult(defaultJobUUID)

	if err == nil {
		t.Error("get computation result must be failed: result is not registered.")
	}

	initialize()
}

// statusもcompletedも存在しない場合にエラーがでるかTest
func TestGetComputationResultFailedEmptyComplated(t *testing.T) {
	initialize()

	os.Mkdir(fmt.Sprintf("/Db/result/%s", defaultJobUUID), 0777)
	data := `{"id":"","job_uuid":"m2db_test_jobuuid","status":1,"result":"[\"1\",\"2\",\"3\"]\","meta":{"piece_id":0}}`
	ioutil.WriteFile(fmt.Sprintf("/Db/result/%s/%d", defaultJobUUID, defaultPieceID), []byte(data), 0666)

	client := Client{}
	_, err := client.GetComputationResult(defaultJobUUID)

	if err == nil {
		t.Error("get computation result must be failed: computation is running(complated file is not found).")
	}

	initialize()
}

// complatedでなくても最新のstatusが正しく取得できるかTest
func TestGetComputationResultSuccessGetStatus(t *testing.T) {
	initialize()

	os.Mkdir(fmt.Sprintf("/Db/result/%s", defaultJobUUID), 0777)
	statusSize := len(pb_types.JobStatus_value)
	// UNKNOWNを除く各Statusについて昇順にチェック
	for i := 1; i < statusSize; i++ {
		status := pb_types.JobStatus_name[int32(i)]
		os.Create(fmt.Sprintf("/Db/result/%s/status_%s", defaultJobUUID, status))

		client := Client{}
		result, err := client.GetComputationResult(defaultJobUUID)

		if err != nil {
			t.Error("get computation result failed: " + err.Error())
		}
		resultIndex := result[0].Status
		if resultIndex != int32(i) {
			t.Errorf("get computation result failed: status must be %s, but value is %s.", status, pb_types.JobStatus_name[resultIndex])
		}
	}

	initialize()
}

/* InsertModelParams(string, string, int32) error */
// モデルパラメータが送れるかTest
func TestInsertModelParamsSuccess(t *testing.T) {
	initialize()

	client := Client{}
	errInsert := client.InsertModelParams(defaultJobUUID, defaultParams, defaultPieceID)

	if errInsert != nil {
		t.Error("insert model params failed: " + errInsert.Error())
	}
	_, errExist := os.Stat(fmt.Sprintf("/Db/result/%s", defaultJobUUID))
	if errExist != nil {
		t.Error("insert model prarams failed: " + errExist.Error())
	}

	initialize()
}

// 重複モデルパラメータを弾くかTest
func TestInsertModelParamsSuccessDuplicate(t *testing.T) {
	initialize()

	client := Client{}
	client.InsertModelParams(defaultJobUUID, defaultParams, defaultPieceID)
	errInsert := client.InsertModelParams(defaultJobUUID, defaultParams, defaultPieceID)

	if errInsert == nil {
		t.Error("insert duplicate model params must be failed, but success.")
	}

	initialize()
}

// pieceが同時に送信されて保存されるかTest
func TestInsertModelParamsParallelSuccess(t *testing.T) {
	initialize()

	client := Client{}
	wg := sync.WaitGroup{}
	for i := 0; i < 100; i++ {
		wg.Add(1)
		go func(pieceId int32) {
			errInsert := client.InsertModelParams(defaultJobUUID, defaultParams, pieceId)
			if errInsert != nil {
				t.Error("insert model params failed: " + errInsert.Error())
			}
			_, errExist := os.Stat(fmt.Sprintf("/Db/result/%s/%d", defaultJobUUID, pieceId))
			if errExist != nil {
				t.Error("insert model params failed: " + errExist.Error())
			}
			defer wg.Done()
		}(int32(i))
	}
	wg.Wait()

	initialize()
}

// 同じShareが同時に送信されてエラーが出るかTest
func TestInsertModelParamsParallelRejectDuplicate(t *testing.T) {
	initialize()

	client := Client{}
	client.InsertModelParams(defaultJobUUID, defaultParams, defaultPieceID)
	wg := sync.WaitGroup{}
	for i := 0; i < 100; i++ {
		wg.Add(1)
		go func() {
			errInsert := client.InsertModelParams(defaultJobUUID, defaultParams, defaultPieceID)
			if errInsert == nil {
				t.Error("insert duplicate shares must be failed, but success.")
			}
			defer wg.Done()
		}()
	}
	wg.Wait()

	initialize()
}

func TestGetElapsedTimeSuccess(t *testing.T) {
	initialize()

	os.Mkdir(fmt.Sprintf("/Db/result/%s", defaultJobUUID), 0777)
	data := `{"id":"","job_uuid":"m2db_test_jobuuid","result":"[\"1\",\"2\",\"3\"]","meta":{"piece_id":0}}`
	ioutil.WriteFile(fmt.Sprintf("/Db/result/%s/%d", defaultJobUUID, defaultPieceID), []byte(data), 0666)
	clock_start := "1666942928259"
	clock_end := "1666943026561"
	os.Create(fmt.Sprintf("/Db/result/%s/completed", defaultJobUUID))
	ioutil.WriteFile(fmt.Sprintf("/Db/result/%s/status_PRE_JOB", defaultJobUUID), []byte(clock_start), 0666)
	ioutil.WriteFile(fmt.Sprintf("/Db/result/%s/status_COMPLETED", defaultJobUUID), []byte(clock_end), 0666)
	expect := 98.302

	client := Client{}
	elapsedTime, err := client.GetElapsedTime(defaultJobUUID)

	if err != nil {
		t.Error("get elapsed time failed: " + err.Error())
	}
	if math.Abs(elapsedTime - expect) > 0.001 {
		t.Errorf("get elapsed time failed: elapsedTime must be %f, but value is %f.",expect,elapsedTime)
	}
}

// XXX: オブジェクトストレージへの移行に備えて廃止予定
/* GetDataList() (string, error) */
