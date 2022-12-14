package mng2db

import (
	"fmt"
	"io/ioutil"
	"math"
	"os"
	"path/filepath"
	"reflect"
	"regexp"
	"sync"
	"testing"

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

var defaultParams = []string{"1", "2", "3"}

var defaultResult = []string{"1", "2", "3"}

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
func TestGetComputationResultSuccessDim1(t *testing.T) {
	initialize()

	os.Mkdir(fmt.Sprintf("/Db/result/%s", defaultJobUUID), 0777)
	data := `{"id":"","job_uuid":"m2db_test_jobuuid","result":["1","2","3"],"meta":{"piece_id":0,"column_number": 3}}`
	ioutil.WriteFile(fmt.Sprintf("/Db/result/%s/dim1_%d", defaultJobUUID, defaultPieceID), []byte(data), 0666)
	os.Create(fmt.Sprintf("/Db/result/%s/completed", defaultJobUUID))
	os.Create(fmt.Sprintf("/Db/result/%s/status_COMPLETED", defaultJobUUID))

	client := Client{}
	result, _, err := client.GetComputationResult(defaultJobUUID, []string{"dim1"})

	if err != nil {
		t.Error("get computation result failed: " + err.Error())
	}
	if result[0].JobUUID != defaultJobUUID {
		t.Error(fmt.Sprintf("get computation result failed: JobUUID must be %s, but value is %s", defaultJobUUID, result[0].JobUUID))
	}
	if !reflect.DeepEqual(result[0].Result, defaultResult) {
		t.Error(fmt.Sprintf("get computation result failed: Result must be %s, but value is %s", defaultResult, result[0].Result))
	}
	if result[0].Meta.PieceID != defaultPieceID {
		t.Error(fmt.Sprintf("get computation result failed: PieceID must be %d, but value is %d", defaultPieceID, result[0].Meta.PieceID))
	}
	if result[0].Status != pb_types.JobStatus_COMPLETED {
		t.Error(fmt.Sprintf("get computation result failed: Status must be %d, but value is %d", pb_types.JobStatus_COMPLETED, result[0].Status))
	}

	initialize()
}
func TestGetComputationResultSuccessDim2(t *testing.T) {
	initialize()

	os.Mkdir(fmt.Sprintf("/Db/result/%s", defaultJobUUID), 0777)
	data := `{"id":"","job_uuid":"m2db_test_jobuuid","result":["1","2","3"],"meta":{"piece_id":0,"column_number": 3}}`
	ioutil.WriteFile(fmt.Sprintf("/Db/result/%s/dim2_%d", defaultJobUUID, defaultPieceID), []byte(data), 0666)
	os.Create(fmt.Sprintf("/Db/result/%s/completed", defaultJobUUID))
	os.Create(fmt.Sprintf("/Db/result/%s/status_COMPLETED", defaultJobUUID))

	client := Client{}
	result, _, err := client.GetComputationResult(defaultJobUUID, []string{"dim2"})

	if err != nil {
		t.Error("get computation result failed: " + err.Error())
	}
	if result[0].JobUUID != defaultJobUUID {
		t.Error(fmt.Sprintf("get computation result failed: JobUUID must be %s, but value is %s", defaultJobUUID, result[0].JobUUID))
	}
	if !reflect.DeepEqual(result[0].Result, defaultResult) {
		t.Error(fmt.Sprintf("get computation result failed: Result must be %s, but value is %s", defaultResult, result[0].Result))
	}
	if result[0].Meta.PieceID != defaultPieceID {
		t.Error(fmt.Sprintf("get computation result failed: PieceID must be %d, but value is %d", defaultPieceID, result[0].Meta.PieceID))
	}
	if result[0].Status != pb_types.JobStatus_COMPLETED {
		t.Error(fmt.Sprintf("get computation result failed: Status must be %d, but value is %d", pb_types.JobStatus_COMPLETED, result[0].Status))
	}

	initialize()
}
func TestGetComputationResultSuccessSchema(t *testing.T) {
	initialize()

	os.Mkdir(fmt.Sprintf("/Db/result/%s", defaultJobUUID), 0777)
	data := `{"id":"","job_uuid":"m2db_test_jobuuid","result":["1","2","3"],"meta":{"piece_id":0,"column_number": 3}}`
	ioutil.WriteFile(fmt.Sprintf("/Db/result/%s/schema_%d", defaultJobUUID, defaultPieceID), []byte(data), 0666)
	os.Create(fmt.Sprintf("/Db/result/%s/completed", defaultJobUUID))
	os.Create(fmt.Sprintf("/Db/result/%s/status_COMPLETED", defaultJobUUID))

	client := Client{}
	result, _, err := client.GetComputationResult(defaultJobUUID, []string{"schema"})

	if err != nil {
		t.Error("get computation result failed: " + err.Error())
	}
	if result[0].JobUUID != defaultJobUUID {
		t.Error(fmt.Sprintf("get computation result failed: JobUUID must be %s, but value is %s", defaultJobUUID, result[0].JobUUID))
	}
	if !reflect.DeepEqual(result[0].Result, defaultResult) {
		t.Error(fmt.Sprintf("get computation result failed: Result must be %s, but value is %s", defaultResult, result[0].Result))
	}
	if result[0].Meta.PieceID != defaultPieceID {
		t.Error(fmt.Sprintf("get computation result failed: PieceID must be %d, but value is %d", defaultPieceID, result[0].Meta.PieceID))
	}
	if result[0].Status != pb_types.JobStatus_COMPLETED {
		t.Error(fmt.Sprintf("get computation result failed: Status must be %d, but value is %d", pb_types.JobStatus_COMPLETED, result[0].Status))
	}

	initialize()
}

// 計算結果のみ存在しない場合にresultの件数が0になるかTest
func TestGetComputationResultFailedEmptyOnlyComputationResult(t *testing.T) {
	initialize()

	os.Mkdir(fmt.Sprintf("/Db/result/%s", defaultJobUUID), 0777)
	os.Create(fmt.Sprintf("/Db/result/%s/completed", defaultJobUUID))
	os.Create(fmt.Sprintf("/Db/result/%s/status_COMPLETED", defaultJobUUID))

	client := Client{}
	result, _, err := client.GetComputationResult(defaultJobUUID, []string{"dim1", "dim2", "schema"})

	if err == nil {
		t.Error("get computation result must be failed")
	}

	if len(result) != 0 {
		t.Errorf("get computation result must be empty, but result is %v", result)
	}

	initialize()
}

// 計算結果が存在しない場合にエラーがでるかTest
func TestGetComputationResultFailedEmptyResult(t *testing.T) {
	initialize()

	client := Client{}
	_, _, err := client.GetComputationResult(defaultJobUUID, []string{"dim1"})

	if err == nil {
		t.Error("get computation result must be failed: result is not registered.")
	}

	initialize()
}

// statusもcompletedも存在しない場合にエラーがでるかTest
func TestGetComputationResultFailedEmptyComplated(t *testing.T) {
	initialize()

	os.Mkdir(fmt.Sprintf("/Db/result/%s", defaultJobUUID), 0777)
	data := `{"id":"","job_uuid":"m2db_test_jobuuid","status":1,"result":["1","2","3"],"meta":{"piece_id":0,"column_number": 3}}`
	ioutil.WriteFile(fmt.Sprintf("/Db/result/%s/%d", defaultJobUUID, defaultPieceID), []byte(data), 0666)

	client := Client{}
	_, _, err := client.GetComputationResult(defaultJobUUID, []string{"dim1"})

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
	// UNKNOWN, ERRORを除く各Statusについて昇順にチェック
	for i := 2; i < statusSize; i++ {
		status := pb_types.JobStatus_name[int32(i)]
		os.Create(fmt.Sprintf("/Db/result/%s/status_%s", defaultJobUUID, status))

		client := Client{}
		result, _, err := client.GetComputationResult(defaultJobUUID, []string{"dim1"})

		if err != nil {
			t.Error("get computation result failed: " + err.Error())
		}
		resultIndex := int32(result[0].Status)
		if resultIndex != int32(i) {
			t.Errorf("get computation result failed: status must be %s, but value is %s.", status, pb_types.JobStatus_name[resultIndex])
		}
	}

	initialize()
}

// ERROR の status が存在する時にエラー情報を取得できるかTest
func TestGetComputationResultFailedJobErrorInfo(t *testing.T) {
	initialize()
	defer initialize()

	os.Mkdir(fmt.Sprintf("/Db/result/%s", defaultJobUUID), 0777)
	data := `{"what": "test"}`
	ioutil.WriteFile(fmt.Sprintf("/Db/result/%s/status_%s", defaultJobUUID, pb_types.JobStatus_ERROR.String()), []byte(data), 0666)

	client := Client{}
	_, info, err := client.GetComputationResult(defaultJobUUID, []string{"dim1"})

	if err != nil {
		t.Error(err)
	}

	if info == nil {
		t.Error("there are no job error info")
	}

	expected := "test"
	if info.What != expected {
		t.Errorf("error information could not be parsed expectedly: required property What: %s, expected: %s", info.What, expected)
	}

	if info.Stacktrace != nil {
		t.Error("error information could not be parsed expectedly: optional property Stacktrace is not nil")
	}
}

// ERROR の status が存在する時にエラー情報(Stacktrace付き)を取得できるかTest
func TestGetComputationResultFailedJobErrorInfoWithStacktrace(t *testing.T) {
	initialize()
	defer initialize()

	os.Mkdir(fmt.Sprintf("/Db/result/%s", defaultJobUUID), 0777)
	data := `{
		"what": "test",
		"stacktrace": {
			"frames": []
		}
	}`
	ioutil.WriteFile(fmt.Sprintf("/Db/result/%s/status_%s", defaultJobUUID, pb_types.JobStatus_ERROR.String()), []byte(data), 0666)

	client := Client{}
	_, info, err := client.GetComputationResult(defaultJobUUID, []string{"dim1"})

	if err != nil {
		t.Error(err)
	}

	if info == nil {
		t.Error("there are no job error info")
	}

	expected := "test"
	if info.What != expected {
		t.Errorf("error information could not be parsed expectedly: required property What: %s, expected: %s", info.What, expected)
	}

	if info.Stacktrace == nil {
		t.Error("error information could not be parsed expectedly: optional property Stacktrace is nil")
	}
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
			_, errExist := os.Stat(fmt.Sprintf("/Db/result/%s/dim1_%d", defaultJobUUID, pieceId))
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
	if math.Abs(elapsedTime-expect) > 0.001 {
		t.Errorf("get elapsed time failed: elapsedTime must be %f, but value is %f.", expect, elapsedTime)
	}
}

/* GetMatchingColumn(string) (int32, error) */
// MatchingColumnが取得できるかTest
func TestGetMatchingColumnSuccess(t *testing.T) {
	initialize()

	os.Mkdir(fmt.Sprintf("/Db/share/%s", defaultDataID), 0777)
	data := `{"meta":{"matching_column":1}}`
	ioutil.WriteFile(fmt.Sprintf("/Db/share/%s/%d", defaultDataID, defaultPieceID), []byte(data), 0666)

	client := Client{}
	matching_column, err := client.GetMatchingColumn(defaultDataID)

	if err != nil {
		t.Error("get matching column failed: " + err.Error())
	}

	var expect int32 = 1
	if matching_column != expect {
		t.Errorf("get matching column failed: matching column must be %v, but value is %v", expect, matching_column)
	}

	initialize()
}

// 保存されていないdata_idでのリクエストでエラーがでるかTest
func TestGetMatchingColumnFailedEmptyID(t *testing.T) {
	initialize()

	client := Client{}
	_, err := client.GetMatchingColumn(defaultDataID)

	if err == nil {
		t.Error("get matching column must be failed: data is not registered.")
	}

	initialize()
}

// XXX: オブジェクトストレージへの移行に備えて廃止予定
/* GetDataList() (string, error) */
