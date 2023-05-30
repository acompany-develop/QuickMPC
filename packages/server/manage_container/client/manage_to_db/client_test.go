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

	pb_types "github.com/acompany-develop/QuickMPC/proto/common_types"
)

// DBを初期化する
func initialize() {
	isGitkeep := regexp.MustCompile(".*.gitkeep")
	files, _ := filepath.Glob("/db/*/*")
	for _, f := range files {
		if !isGitkeep.MatchString(f) {
			os.RemoveAll(f)
		}
	}
}

var defaultSchema = []*pb_types.Schema{
	{Name: "attr1", Type: pb_types.ShareValueTypeEnum_SHARE_VALUE_TYPE_FIXED_POINT},
	{Name: "attr2", Type: pb_types.ShareValueTypeEnum_SHARE_VALUE_TYPE_FIXED_POINT},
	{Name: "attr2", Type: pb_types.ShareValueTypeEnum_SHARE_VALUE_TYPE_FIXED_POINT},
}

const defaultDataID = "m2db_test_dataid"
const defaultPieceID = 0
const defaultShares = `[["1","2","3"],["4","5","6"]]`
const defaultSentAt = ""
const defaultMatchingColumn = 1
const defaultJobUUID = "m2db_test_jobuuid"

var defaultParams = []string{"1", "2", "3"}

var defaultResult = []string{"1", "2", "3"}

func TestGetSharePieceSize(t *testing.T) {
	initialize()

	testcases := map[string]struct {
		dataID   string
		size     int32
		expected int32
	}{
		"1":   {"TestGetSharePieceSize1", 1, 1},
		"10":  {"TestGetSharePieceSize10", 10, 10},
		"100": {"TestGetSharePieceSize100", 100, 100},
	}
	for name, tt := range testcases {
		tt := tt
		t.Run(name, func(t *testing.T) {
			t.Parallel()
			os.Mkdir(fmt.Sprintf("/db/share/%s", tt.dataID), 0777)
			for i := 0; i < int(tt.size); i++ {
				os.Create(fmt.Sprintf("/db/share/%s/%d", tt.dataID, i))
			}

			client := Client{}
			size, err := client.GetSharePieceSize(tt.dataID)
			if size != tt.expected {
				t.Errorf("size must be %d, but %d", tt.expected, size)
			}
			if err != nil {
				t.Error(err)
			}
		})
	}
}
func TestGetSharePieceSizeFailed(t *testing.T) {
	initialize()

	generateDataID := "TestGetSharePieceSizeFailed"
	os.Mkdir(fmt.Sprintf("/db/share/%s", generateDataID), 0777)

	testcases := map[string]struct {
		dataID   string
		expected string
	}{
		"dir_empty":   {"empty_dir_name", "データ未登録エラー: empty_dir_nameは登録されていません．"},
		"piece_empty": {generateDataID, "データ未登録エラー: TestGetSharePieceSizeFailedは登録されていません．"},
	}
	for name, tt := range testcases {
		tt := tt
		t.Run(name, func(t *testing.T) {
			t.Parallel()
			fmt.Println(tt.dataID)

			client := Client{}
			_, err := client.GetSharePieceSize(tt.dataID)
			if err.Error() != tt.expected {
				t.Errorf("error message must be `%s`, but `%s`", tt.expected, err.Error())
			}
		})
	}
}

/* InsertShares(string, []string, int32, string, string) error */
// シェアが保存されるかTest
func TestInsertSharesSuccess(t *testing.T) {
	initialize()

	client := Client{}
	errInsert := client.InsertShares(defaultDataID, defaultSchema, defaultPieceID, defaultShares, defaultSentAt, defaultMatchingColumn)

	if errInsert != nil {
		t.Error("insert shares failed: " + errInsert.Error())
	}
	_, errExist := os.Stat(fmt.Sprintf("/db/share/%s/%d", defaultDataID, defaultPieceID))
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
			_, errExist := os.Stat(fmt.Sprintf("/db/share/%s/%d", defaultDataID, pieceId))
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

	os.Mkdir(fmt.Sprintf("/db/share/%s", defaultDataID), 0777)
	var dataIds = []string{defaultDataID}

	client := Client{}
	client.DeleteShares(dataIds)

	_, errExist := os.Stat(fmt.Sprintf("/db/share/%s", defaultDataID))
	if errExist == nil {
		t.Error(fmt.Sprintf("delete shares failed: '/db/share/%s' must be deleted, but exist", defaultDataID))
	}
	initialize()
}

func TestGetSharePiece(t *testing.T) {
	initialize()

	testcases := map[string]struct {
		dataID   string
		data     string
		expected Share
	}{
		"full": {
			"TestGetSharePieceFull",
			`{"value":[["1","2"],["3","4"]],"meta":{"piece_id":0,"matching_column": 1}}`,
			Share{Value: [][]string{{"1", "2"}, {"3", "4"}}, Meta: ShareMeta{PieceID: 0, MatchingColumn: 1}},
		},
		"empty": {
			"TestGetSharePieceEmpty",
			`{}`,
			Share{},
		},
	}
	for name, tt := range testcases {
		tt := tt
		t.Run(name, func(t *testing.T) {
			t.Parallel()

			os.Mkdir(fmt.Sprintf("/db/share/%s", tt.dataID), 0777)
			ioutil.WriteFile(fmt.Sprintf("/db/share/%s/%d", tt.dataID, 0), []byte(tt.data), 0666)

			client := Client{}
			share, err := client.GetSharePiece(tt.dataID, 0)
			if err != nil {
				t.Error(err)
			}
			if !reflect.DeepEqual(share, tt.expected) {
				t.Errorf("share must be %v, but %v", tt.expected, share)
			}
		})
	}

	initialize()
}

/* GetSchema(string) ([]string, error) */
// Schemaが取得できるかTest
func TestGetSchemaSuccess(t *testing.T) {
	initialize()

	os.Mkdir(fmt.Sprintf("/db/share/%s", defaultDataID), 0777)
	data := `{"meta":{"schema":[
		{"name": "attr1"},
		{"name": "attr2"},
		{"name": "attr3"}
	]}}`
	ioutil.WriteFile(fmt.Sprintf("/db/share/%s/%d", defaultDataID, defaultPieceID), []byte(data), 0666)

	client := Client{}
	schema, err := client.GetSchema(defaultDataID)

	if err != nil {
		t.Error("get schema failed: " + err.Error())
	}
	cor := []*pb_types.Schema{{Name: "attr1"}, {Name: "attr2"}, {Name: "attr3"}}
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

	os.Mkdir(fmt.Sprintf("/db/result/%s", defaultJobUUID), 0777)
	data := `{"id":"","job_uuid":"m2db_test_jobuuid","result":["1","2","3"],"meta":{"piece_id":0,"column_number": 3}}`
	ioutil.WriteFile(fmt.Sprintf("/db/result/%s/dim1_%d", defaultJobUUID, defaultPieceID), []byte(data), 0666)
	os.Create(fmt.Sprintf("/db/result/%s/completed", defaultJobUUID))
	os.Create(fmt.Sprintf("/db/result/%s/status_COMPLETED", defaultJobUUID))

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

	os.Mkdir(fmt.Sprintf("/db/result/%s", defaultJobUUID), 0777)
	data := `{"id":"","job_uuid":"m2db_test_jobuuid","result":["1","2","3"],"meta":{"piece_id":0,"column_number": 3}}`
	ioutil.WriteFile(fmt.Sprintf("/db/result/%s/dim2_%d", defaultJobUUID, defaultPieceID), []byte(data), 0666)
	os.Create(fmt.Sprintf("/db/result/%s/completed", defaultJobUUID))
	os.Create(fmt.Sprintf("/db/result/%s/status_COMPLETED", defaultJobUUID))

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

	os.Mkdir(fmt.Sprintf("/db/result/%s", defaultJobUUID), 0777)
	data := `{"id":"","job_uuid":"m2db_test_jobuuid","result":["1","2","3"],"meta":{"piece_id":0,"column_number": 3}}`
	ioutil.WriteFile(fmt.Sprintf("/db/result/%s/schema_%d", defaultJobUUID, defaultPieceID), []byte(data), 0666)
	os.Create(fmt.Sprintf("/db/result/%s/completed", defaultJobUUID))
	os.Create(fmt.Sprintf("/db/result/%s/status_COMPLETED", defaultJobUUID))

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

	os.Mkdir(fmt.Sprintf("/db/result/%s", defaultJobUUID), 0777)
	os.Create(fmt.Sprintf("/db/result/%s/completed", defaultJobUUID))
	os.Create(fmt.Sprintf("/db/result/%s/status_COMPLETED", defaultJobUUID))

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

	os.Mkdir(fmt.Sprintf("/db/result/%s", defaultJobUUID), 0777)
	data := `{"id":"","job_uuid":"m2db_test_jobuuid","status":1,"result":["1","2","3"],"meta":{"piece_id":0,"column_number": 3}}`
	ioutil.WriteFile(fmt.Sprintf("/db/result/%s/%d", defaultJobUUID, defaultPieceID), []byte(data), 0666)

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

	os.Mkdir(fmt.Sprintf("/db/result/%s", defaultJobUUID), 0777)
	statusSize := len(pb_types.JobStatus_value)
	// UNKNOWN, ERRORを除く各Statusについて昇順にチェック
	for i := 2; i < statusSize; i++ {
		status := pb_types.JobStatus_name[int32(i)]
		os.Create(fmt.Sprintf("/db/result/%s/status_%s", defaultJobUUID, status))

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

	os.Mkdir(fmt.Sprintf("/db/result/%s", defaultJobUUID), 0777)
	data := `{"what": "test"}`
	ioutil.WriteFile(fmt.Sprintf("/db/result/%s/status_%s", defaultJobUUID, pb_types.JobStatus_ERROR.String()), []byte(data), 0666)

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

	os.Mkdir(fmt.Sprintf("/db/result/%s", defaultJobUUID), 0777)
	data := `{
		"what": "test",
		"stacktrace": {
			"frames": []
		}
	}`
	ioutil.WriteFile(fmt.Sprintf("/db/result/%s/status_%s", defaultJobUUID, pb_types.JobStatus_ERROR.String()), []byte(data), 0666)

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

// ERROR の status が存在する時にエラー情報(Stacktrace付き)を取得できるかTest
func TestGetJobErrorInfoSuccess(t *testing.T) {
	initialize()
	defer initialize()

	os.Mkdir(fmt.Sprintf("/db/result/%s", defaultJobUUID), 0777)
	data := `{
		"what": "test",
		"stacktrace": {
			"frames": []
		}
	}`
	ioutil.WriteFile(fmt.Sprintf("/db/result/%s/status_%s", defaultJobUUID, pb_types.JobStatus_ERROR.String()), []byte(data), 0666)

	client := Client{}
	info, err := client.GetJobErrorInfo(defaultJobUUID)

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

// statusもcompletedも存在しない場合にエラーがでるかTest
func TestGetJobErrorInfoFailed(t *testing.T) {
	initialize()
	defer initialize()

	client := Client{}
	_, err := client.GetJobErrorInfo(defaultJobUUID)

	if err == nil {
		t.Error("get job error info must be failed: any status file not found")
	}
}

func TestGetElapsedTimeSuccess(t *testing.T) {
	initialize()

	os.Mkdir(fmt.Sprintf("/db/result/%s", defaultJobUUID), 0777)
	data := `{"id":"","job_uuid":"m2db_test_jobuuid","result":"[\"1\",\"2\",\"3\"]","meta":{"piece_id":0}}`
	ioutil.WriteFile(fmt.Sprintf("/db/result/%s/%d", defaultJobUUID, defaultPieceID), []byte(data), 0666)
	clock_start := "1666942928259"
	clock_end := "1666943026561"
	os.Create(fmt.Sprintf("/db/result/%s/completed", defaultJobUUID))
	ioutil.WriteFile(fmt.Sprintf("/db/result/%s/status_PRE_JOB", defaultJobUUID), []byte(clock_start), 0666)
	ioutil.WriteFile(fmt.Sprintf("/db/result/%s/status_COMPLETED", defaultJobUUID), []byte(clock_end), 0666)
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

	os.Mkdir(fmt.Sprintf("/db/share/%s", defaultDataID), 0777)
	data := `{"meta":{"matching_column":1}}`
	ioutil.WriteFile(fmt.Sprintf("/db/share/%s/%d", defaultDataID, defaultPieceID), []byte(data), 0666)

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

// status_RECEIVEDが生成されるかテスト
func TestCreateStatusFile(t *testing.T) {
	initialize()

	client := Client{}
	err := client.CreateStatusFile(defaultJobUUID)
	if err != nil {
		t.Error("create status file failed: " + err.Error())
	}

	_, errExist := os.Stat(fmt.Sprintf("/db/result/%s/status_%s", defaultJobUUID, pb_types.JobStatus_RECEIVED.String()))
	if errExist != nil {
		t.Error("create status file failed: " + errExist.Error())
	}

	initialize()
}

// jobUUIDのディレクトリが削除されるかテスト
func TestDeleteStatusFile(t *testing.T) {
	initialize()

	path := fmt.Sprintf("/db/result/%s", defaultJobUUID)
	os.Mkdir(path, 0777)
	fp, _ := os.Create(fmt.Sprintf("%s/status_%s", path, pb_types.JobStatus_RECEIVED.String()))
	fp.Close()

	client := Client{}
	err := client.DeleteStatusFile(defaultJobUUID)
	if err != nil {
		t.Error("delete status file failed: " + err.Error())
	}

	_, errExist := os.Stat(fmt.Sprintf("/db/result/%s", defaultJobUUID))
	if errExist == nil {
		t.Error(fmt.Sprintf("%s directory must be deleted", defaultJobUUID))
	}

	initialize()
}

// XXX: オブジェクトストレージへの移行に備えて廃止予定
/* GetDataList() (string, error) */
