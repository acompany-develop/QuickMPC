package integration_db_test

import (
	"encoding/json"
	"fmt"
	"sync"
	"testing"

	m2db "github.com/acompany-develop/QuickMPC/src/ManageContainer/Client/ManageToDbGate"
	uft "github.com/acompany-develop/QuickMPC/src/ManageContainer/IntegrationTest/UtilsForTest"
	. "github.com/acompany-develop/QuickMPC/src/ManageContainer/Log"
	utils "github.com/acompany-develop/QuickMPC/src/ManageContainer/Utils"
	"google.golang.org/grpc"
)

var conn *grpc.ClientConn

func init() {
	config, err := utils.GetConfig()
	DbgIp := config.Containers.Dbg
	if err != nil {
		AppLogger.Error(err)
	}
	conn, err = grpc.Dial(DbgIp.Host, grpc.WithInsecure())
	if err != nil {
		AppLogger.Fatalf("did not connect: %v", err)
	}
}

// Test用のn1qlクエリ生成関数
func insert_query(bucket string, id_name string, id string, value string) string {
	id_name_esc := utils.EscapeInjection(id_name, utils.Where)
	id_esc := utils.EscapeInjection(id, utils.Where)
	return fmt.Sprintf("INSERT INTO `%s`"+
		"(KEY, VALUE) VALUES (UUID(), {'%s': '%s', 'result' : '%s'})"+
		"RETURNING *;", bucket, id_name_esc, id_esc, value)
}
func select_query(bucket string, id_name string, id string) string {
	id_name_esc := utils.EscapeInjection(id_name, utils.Where)
	id_esc := utils.EscapeInjection(id, utils.Where)
	return fmt.Sprintf("SELECT x.*, meta().id "+
		"FROM `%s` x WHERE x.%s='%s';", bucket, id_name_esc, id_esc)
}
func delete_query(bucket string, id_name string, id string) string {
	id_name_esc := utils.EscapeInjection(id_name, utils.Where)
	id_esc := utils.EscapeInjection(id, utils.Where)
	return fmt.Sprintf("DELETE FROM `%s` x "+
		"WHERE x.%s='%s' RETURNING x.*, meta().id ;", bucket, id_name_esc, id_esc)
}

func TestExecuteQuery(t *testing.T) {
	id := "executeQueryID"
	uft.DeleteId(t, id)
	bucket := "result"

	_, err := m2db.ExecuteQuery(conn, insert_query(bucket, "id", id, "[\"1\"]"))
	if err != nil {
		t.Error("insert query faild: " + err.Error())
	}

	_, err = m2db.ExecuteQuery(conn, select_query(bucket, "id", id))
	if err != nil {
		t.Error("select query faild: " + err.Error())
	}

	_, err = m2db.ExecuteQuery(conn, delete_query(bucket, "id", id))
	if err != nil {
		t.Error("delete query faild: " + err.Error())
	}
	uft.DeleteId(t, id)
}

func testInsertShares(t *testing.T, dataID string) {
	uft.DeleteId(t, dataID)
	// share送信
	client := m2db.Client{}
	schema := []string{"attr1", "attr2", "attr3"}
	const pieceID int32 = 1
	shares := fmt.Sprintf("[[\"1\",\"2\",\"3\"],[\"4\",\"5\",\"6\"]]")
	sent_at := ""
	err := client.InsertShares(dataID, schema, pieceID, shares, sent_at)
	if err != nil {
		t.Error("insert shares faild: " + err.Error())
	}

	// shareを取り出して比較
	response, _ := m2db.ExecuteQuery(conn, delete_query("share", "data_id", dataID))
	var deleteResult []m2db.Share
	err = json.Unmarshal([]byte(response), &deleteResult)
	if err != nil {
		t.Error("json parses faild: " + err.Error())
	}

	bytes, err := json.Marshal(deleteResult[0].Value)
	if err != nil {
		t.Error(err)
	}

	if string(bytes) != shares {
		t.Error("insert shares test faild: " + err.Error())
	}
	uft.DeleteId(t, dataID)
}
func TestInsertSharesSerial(t *testing.T) {
	for i := 0; i < 10; i++ {
		dataID := fmt.Sprintf("insertSharesSerial%d", i)
		testInsertShares(t, dataID)
	}
}
func TestInsertSharesParallel(t *testing.T) {
	wg := sync.WaitGroup{}
	for i := 0; i < 10; i++ {
		wg.Add(1)
		go func(i int) {
			dataID := fmt.Sprintf("insertSharesParallel%d", i)
			testInsertShares(t, dataID)
			defer wg.Done()
		}(i)
	}
	wg.Wait()
}

func testGetSchema(t *testing.T, dataID string) {
	uft.DeleteId(t, dataID)
	client := m2db.Client{}
	// share送信
	schema := []string{"attr1", "attr2", fmt.Sprintf("attr%s", dataID)}
	const pieceID int32 = 1
	const shares string = "[[\"1\",\"2\",\"3\"],[\"4\",\"5\",\"6\"]]"
	const sent_at string = ""
	err := client.InsertShares(dataID, schema, pieceID, shares, sent_at)
	if err != nil {
		t.Error("insert shares faild: " + err.Error())
	}

	// schema取得して比較
	response, err := client.GetSchema(dataID)

	if err != nil {
		t.Error("get schema failed: " + err.Error())
	}

	for i := 0; i < len(schema); i++ {
		if response[i] != schema[i] {
			t.Error("Response not equal expected schema")
		}
	}
	uft.DeleteId(t, dataID)
}
func TestGetSchemaSerial(t *testing.T) {
	for i := 0; i < 10; i++ {
		dataID := fmt.Sprintf("getSchemaSerial%d", i)
		testGetSchema(t, dataID)
	}
}
func TestGetSchemaParallel(t *testing.T) {
	wg := sync.WaitGroup{}
	for i := 0; i < 10; i++ {
		wg.Add(1)
		go func(i int) {
			dataID := fmt.Sprintf("getSchemaParallel%d", i)
			testGetSchema(t, dataID)
			defer wg.Done()
		}(i)
	}
	wg.Wait()
}

func testGetComputationResult(t *testing.T, dataID string) {
	uft.DeleteId(t, dataID)
	bucket := "result"
	value := fmt.Sprintf("[\"1\",\"2\",\"3\"]")

	client := m2db.Client{}
	_, err := m2db.ExecuteQuery(conn, insert_query(bucket, "job_uuid", dataID, value))
	if err != nil {
		t.Error("insert result failed: " + err.Error())
	}

	var computationResult []*m2db.ComputationResult
	computationResult, err = client.GetComputationResult(dataID)

	if err != nil {
		t.Error("get computation result failed: " + err.Error())
	}
	result := computationResult[0].Result

	if result != value {
		t.Errorf("Result not equal %v, Result: %v", value, result)
	}
	uft.DeleteId(t, dataID)
}
func TestGetComputationResultSerial(t *testing.T) {
	for i := 0; i < 10; i++ {
		dataID := fmt.Sprintf("GetComputationResultSerial%d", i)
		testGetComputationResult(t, dataID)
	}
}
func TestGetComputationResultParallel(t *testing.T) {
	wg := sync.WaitGroup{}
	for i := 0; i < 10; i++ {
		wg.Add(1)
		go func(i int) {
			dataID := fmt.Sprintf("GetComputationResultParallel%d", i)
			testGetComputationResult(t, dataID)
			defer wg.Done()
		}(i)
	}
	wg.Wait()
}

func TestCountShares(t *testing.T) {
	const dataID1 string = "conutshares1"
	const dataID2 string = "conutshares2"
	uft.DeleteId(t, dataID1)
	uft.DeleteId(t, dataID2)
	client := m2db.Client{}
	schema := []string{"attr1", "attr2", "attr3"}
	const pieceID1 int32 = 1
	const pieceID2 int32 = 2
	const shares string = "[[\"1\",\"2\",\"3\"],[\"4\",\"5\",\"6\"]]"
	const sent_at string = ""
	// share送信
	err := client.InsertShares(dataID1, schema, pieceID1, shares, sent_at)
	if err != nil {
		t.Error("insert shares faild: " + err.Error())
	}
	// count == 1
	cnt, err := client.Count("data_id", dataID1, "share")
	if err != nil {
		t.Error("count shares faild: " + err.Error())
	}
	if cnt != 1 {
		t.Error("count not equal 1, count: " + fmt.Sprintf("%d", cnt))
	}

	// share送信(同じデータなのでエラーが出る)
	err = client.InsertShares(dataID1, schema, pieceID1, shares, sent_at)
	if err == nil {
		t.Error("same data insert error")
	}

	// share送信(pieceIDが違うので登録される)
	err = client.InsertShares(dataID1, schema, pieceID2, shares, sent_at)
	if err != nil {
		t.Error("insert shares faild: " + err.Error())
	}
	// count == 2
	cnt, err = client.Count("data_id", dataID1, "share")
	if err != nil {
		t.Error("count shares faild: " + err.Error())
	}
	if cnt != 2 {
		t.Error("count not equal 2, count: " + fmt.Sprintf("%d", cnt))
	}

	// share送信(dataIDが違うので登録される)
	err = client.InsertShares(dataID2, schema, pieceID1, shares, sent_at)
	if err != nil {
		t.Error("insert shares faild: " + err.Error())
	}
	// count == 1
	cnt, err = client.Count("data_id", dataID2, "share")
	if err != nil {
		t.Error("count shares faild: " + err.Error())
	}
	if cnt != 1 {
		t.Error("count not equal 1, count: " + fmt.Sprintf("%d", cnt))
	}
	uft.DeleteId(t, dataID1)
	uft.DeleteId(t, dataID2)
}

func testInsertModelParams(t *testing.T, jobUUID string) {
	uft.DeleteId(t, jobUUID)
	client := m2db.Client{}
	// params送信
	params := fmt.Sprintf("[\"1\",\"2\",\"3\"]")
	err := client.InsertModelParams(jobUUID, params, 1)
	if err != nil {
		t.Error("insert model params faild: " + err.Error())
	}

	// 重複データは送信されない
	err = client.InsertModelParams(jobUUID, params, 1)
	if err == nil {
		t.Error("duplicate data insert error")
	}

	// paramを取り出して比較
	response, _ := m2db.ExecuteQuery(conn, delete_query("result", "job_uuid", jobUUID))

	var deleteResult []m2db.ComputationResult
	err = json.Unmarshal([]byte(response), &deleteResult)
	if err != nil {
		t.Error("json parses faild: " + err.Error())
	}

	if deleteResult[0].Result != params {
		t.Error("insert model params test faild: " + err.Error())
	}
	uft.DeleteId(t, jobUUID)
}
func TestInsertModelParamsSerial(t *testing.T) {
	for i := 0; i < 10; i++ {
		jobUUID := fmt.Sprintf("InsertModelParamsSerial%d", i)
		testInsertModelParams(t, jobUUID)
	}
}
func TestInsertModelParamsParallel(t *testing.T) {
	wg := sync.WaitGroup{}
	for i := 0; i < 10; i++ {
		wg.Add(1)
		go func(i int) {
			JobUUID := fmt.Sprintf("InsertModelParamsParallel%d", i)
			testInsertModelParams(t, JobUUID)
			defer wg.Done()
		}(i)
	}
	wg.Wait()
}

// piece分割されたmodel paramが正常にinsertされるか
func TestInsertModelParamsPiece(t *testing.T) {
	jobUUID := "testInsertModelParamsPiece"
	uft.DeleteId(t, jobUUID)
	client := m2db.Client{}

	// params送信
	params1 := fmt.Sprintf("[\"1\",\"2\",\"3\"]")
	err := client.InsertModelParams(jobUUID, params1, 1)
	if err != nil {
		t.Error("insert model params1 faild: " + err.Error())
	}

	params2 := fmt.Sprintf("[\"4\",\"5\",\"6\"]")
	err = client.InsertModelParams(jobUUID, params2, 2)
	if err != nil {
		t.Error("insert model params1 faild: " + err.Error())
	}

	// paramを取り出して比較
	response, _ := m2db.ExecuteQuery(conn, delete_query("result", "job_uuid", jobUUID))

	var deleteResult []m2db.ComputationResult
	err = json.Unmarshal([]byte(response), &deleteResult)
	if err != nil {
		t.Error("json parses faild: " + err.Error())
	}

	if deleteResult[0].Result != params1 {
		t.Error("insert model params test faild: " + err.Error())
	}
	if deleteResult[1].Result != params2 {
		t.Error("insert model params test faild: " + err.Error())
	}
	uft.DeleteId(t, jobUUID)
}

func testInsertPiece(t *testing.T, dataID string, pieces []int) {
	uft.DeleteId(t, dataID)
	client := m2db.Client{}
	schema := []string{"attr1", "attr2", "attr3"}

	for _, i := range pieces {
		pieceID := int32(i)
		shares := fmt.Sprintf("[[\"%d\"]]", pieceID)
		sent_at := ""
		err := client.InsertShares(dataID, schema, pieceID, shares, sent_at)
		if err != nil {
			t.Error("insert shares faild: " + err.Error())
		}
	}

	// shareを取り出して比較
	response, _ := m2db.ExecuteQuery(conn, delete_query("share", "data_id", dataID))
	var deleteResult []m2db.Share
	err := json.Unmarshal([]byte(response), &deleteResult)
	if err != nil {
		t.Error("json parses faild: " + err.Error())
	}

	for _, res := range deleteResult {
		pieceID := res.Meta.PieceID
		bytes, err := json.Marshal(res.Value)
		if err != nil {
			t.Error(err)
		}
		if string(bytes) != fmt.Sprintf("[[\"%d\"]]", pieceID) {
			t.Error("Response not equal expected piece")
		}
	}
	uft.DeleteId(t, dataID)
}

func TestInsertPieceSerial(t *testing.T) {
	for i := 0; i < 10; i++ {
		dataID := fmt.Sprintf("InsertPieceSerial%d", i)
		testInsertPiece(t, dataID, []int{1, 2, 3, 4, 5})
	}
}
func TestInsertPieceParallelOrder(t *testing.T) {
	wg := sync.WaitGroup{}
	for i := 0; i < 10; i++ {
		wg.Add(1)
		go func(i int) {
			dataID := fmt.Sprintf("InsertPieceParallelOrder%d", i)
			testInsertPiece(t, dataID, []int{1, 2, 3, 4, 5})
			defer wg.Done()
		}(i)
	}
	wg.Wait()
}
func TestInsertPieceParallelReverse(t *testing.T) {
	wg := sync.WaitGroup{}
	for i := 0; i < 10; i++ {
		wg.Add(1)
		go func(i int) {
			dataID := fmt.Sprintf("InsertPieceParallelReverse%d", i)
			testInsertPiece(t, dataID, []int{5, 4, 3, 2, 1})
			defer wg.Done()
		}(i)
	}
	wg.Wait()
}
