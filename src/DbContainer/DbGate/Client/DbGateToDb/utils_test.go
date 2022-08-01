package dbg2db_test

import (
	"fmt"
	"time"

	dbg2db "github.com/acompany-develop/QuickMPC/src/DbContainer/DbGate/Client/DbGateToDb"
	. "github.com/acompany-develop/QuickMPC/src/DbContainer/DbGate/Log"
)

var (
	hostShareDb string     = "sharedb" // jsonのkeyは全て小文字
	testOpts    [][]string = [][]string{{hostShareDb, "share"}}
)

func executeInit(host string, bucket string, funcName string) error {

	// 初期化処理
	// 前回Failしているとデータが残りSELECTで必ず落ちるのでDELETEをしておく
	initQuery := fmt.Sprintf("DELETE FROM `%s` x WHERE x.test_id = \"%s\";", bucket, funcName)
	_, err := dbg2db.ExecuteQuery(&dbg2db.QueryOpts{Host: host, Query: initQuery})
	return err
}

// 推移律が成り立つcasを返す
func getCas() (f func() uint64) {
	var precas uint64 = 0
	var cas uint64 = 0
	var counter uint16 = 0

	getTimeStamp := func() uint64 {
		t := time.Now()
		return uint64(t.UnixNano())
	}

	getLogicalCounter := func() uint16 {
		counter++
		return counter
	}

	// とりあえずcasを生成する
	createCas := func() uint64 {
		timestamp := getTimeStamp()
		logicalcounter := uint64(getLogicalCounter())
		var mask uint64 = ((1 << 64) - 1) - ((1 << 16) - 1)

		// 64bit timestampの下位16bitを捨てたものとlogicalcounterの下位16bitをOR
		var cas uint64 = (mask & timestamp) |
			(((1 << 16) - 1) & logicalcounter)

		return cas
	}

	f = func() uint64 {

		cas = createCas()

		for precas != 0 && cas < precas {
			cas = createCas()
		}

		precas = cas
		return cas
	}

	return
}

func insert(host string, bucket string, testId string, value string) (string, error) {
	// 今回のテストで使用するクエリ
	query := fmt.Sprintf("INSERT INTO `%s` (KEY, VALUE) VALUES %s", bucket, value)
	result, err := dbg2db.ExecuteQuery(&dbg2db.QueryOpts{Host: host, Query: query})
	// AppLogger.Info("%s: %s", testId, result)
	return result, err
}

func _select(host string, bucket string, testId string, amount int) (string, error) {
	// 今回のテストで使用するクエリ
	query := fmt.Sprintf("SELECT x.*, meta(x).id FROM `%s` x USE INDEX (test_all_idx USING GSI) WHERE x.test_id = \"%s\" LIMIT %d", bucket, testId, amount)
	result, err := dbg2db.ExecuteQuery(&dbg2db.QueryOpts{Host: host, Query: query})
	if err != nil {
		AppLogger.Errorf("[SELECT]result: %s\nerror: %s", result, err)
	}
	return result, err
}

func delete(host string, bucket string, testId string, amount int) (string, error) {
	// 今回のテストで使用するクエリ
	query := fmt.Sprintf("DELETE FROM `%s` x WHERE x.test_id = \"%s\" LIMIT %d", bucket, testId, amount)
	result, err := dbg2db.ExecuteQuery(&dbg2db.QueryOpts{Host: host, Query: query})
	if err != nil {
		AppLogger.Errorf("[DELETE]result: %s\nerror: %s", result, err)
	}
	return result, err
}

func deleteRet(host string, bucket string, testId string, amount int) (string, error) {
	// 今回のテストで使用するクエリ
	query := fmt.Sprintf("DELETE FROM `%s` x WHERE x.test_id = \"%s\" LIMIT %d RETURNING x.*, meta(x).id", bucket, testId, amount)
	result, err := dbg2db.ExecuteQuery(&dbg2db.QueryOpts{Host: host, Query: query})
	if err != nil {
		AppLogger.Errorf("[DELETE]result: %s\nerror: %s", result, err)
	}
	return result, err
}
