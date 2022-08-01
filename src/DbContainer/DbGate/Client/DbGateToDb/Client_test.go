package dbg2db_test

import (
	"encoding/json"
	"fmt"
	"math/rand"
	"strconv"
	"strings"
	"testing"

	dbg2db "github.com/acompany-develop/QuickMPC/src/DbContainer/DbGate/Client/DbGateToDb"
	utils "github.com/acompany-develop/QuickMPC/src/DbContainer/DbGate/Utils"
)

// TODO: PCの廃止によりtriple系のN1QLはTest不要になったため, 代わりにshare系のN1QLをTestするように変更
func testExecuteQuery(t *testing.T, tripleN int, forN int) {
	t.Helper()
	funcName := fmt.Sprintf("TestExecuteQuery%d-%d", tripleN, forN)

	host, bucket := testOpts[0][0], testOpts[0][1]
	// 初期化処理
	err := executeInit(host, bucket, funcName)
	if err != nil {
		t.Fatal(err)
	}

	// 今回のテストで使用するクエリ群
	valueList := make([]map[string]string, forN*tripleN)
	insertQueriesList := make([]string, forN)
	f := getCas()
	for i := 0; i < forN; i++ {
		values := make([]string, tripleN)
		for j := 0; j < tripleN; j++ {
			// Insertする値
			valueMap := map[string]string{
				"test_id": funcName,
				"a":       fmt.Sprint(rand.Intn(100000)),
				"b":       fmt.Sprint(rand.Intn(100000)),
				"c":       fmt.Sprint(rand.Intn(100000)),
			}
			// Mapを文字列に変換
			value, _ := utils.ConvertStructToJsonString(valueMap)
			cas := f()
			convertedCas := strconv.FormatUint(cas, 10)
			values[j] = fmt.Sprintf("(\"%s\",%s)", convertedCas, value)
			tempValueMap := map[string]string{
				"a":  valueMap["a"],
				"b":  valueMap["b"],
				"c":  valueMap["c"],
				"id": convertedCas,
			}
			valueList[tripleN*i+j] = tempValueMap
		}

		insertQueriesList[i] = fmt.Sprintf("INSERT INTO `%s` (KEY, VALUE) VALUES %s;", bucket, strings.Join(values, ", "))
	}

	for _, query := range insertQueriesList {
		// Query実行
		_, err := dbg2db.ExecuteQuery(&dbg2db.QueryOpts{Host: host, Query: query})
		if err != nil {
			t.Fatal(err)
		}
	}

	deleteQuery := fmt.Sprintf("DELETE FROM `%s` x WHERE x.test_id = \"%s\" LIMIT %d RETURNING meta(x).id, x.a, x.b, x.c", bucket, funcName, tripleN)
	j := 0
	for i := 0; i < forN; i++ {
		result, err := dbg2db.ExecuteQuery(&dbg2db.QueryOpts{Host: host, Query: deleteQuery})
		if err != nil {
			t.Fatal(err)
		}

		// 結果を"[結果]"の形で受け取るのでSliceへ変更
		resultMapSlice := []map[string]interface{}{}
		err = utils.ConvertJsonStringToStruct(result, &resultMapSlice)
		if err != nil {
			t.Fatal(err)
		}

		for _, resultMap := range resultMapSlice {
			// 結果を確認
			for key, value := range valueList[j] {
				if resultMap[key] != value {
					t.Fatal(fmt.Sprintf("%s returns invalid %s item", deleteQuery[:6], key))
				}
			}
			j++
		}
	}
	// 初期化処理
	err = executeInit(host, bucket, funcName)
	if err != nil {
		t.Fatal(err)
	}
}

func executeInsertQueryForParallel(t *testing.T, host string, insertQueriesList []string) {
	t.Helper()
	for _, query := range insertQueriesList {
		// Query実行
		_, err := dbg2db.ExecuteQuery(&dbg2db.QueryOpts{Host: host, Query: query})
		if err != nil {
			t.Fatal(err)
		}
	}
}

func executeDeleteQueryForParallel(t *testing.T, host string, query string, valueList []map[string]interface{}, forN int, N int, n int) {
	t.Helper()
	j := 0
	for i := 0; i < forN; i++ {
		// for _, valueMap := range valueList {
		result, err := dbg2db.ExecuteQuery(&dbg2db.QueryOpts{Host: host, Query: query})
		if err != nil {
			t.Fatal(err)
		}
		// 結果ログ
		// t.Log(result)

		// 結果を"[結果]"の形で受け取るのでSliceへ変更
		type DeleteTripleResult struct {
			Id string `required:"true" json:"id"`
			A  int    `required:"true" json:"a"`
			B  int    `required:"true" json:"b"`
			C  int    `required:"true" json:"c"`
			P  uint16 `required:"true" json:"p"`
		}
		resultStructs := []DeleteTripleResult{}
		err = utils.ConvertJsonStringToStruct(result, &resultStructs)
		if err != nil {
			t.Fatal(err)
		}

		if len(resultStructs) == 0 {
			t.Fatal(err)
		}
		for _, resultStruct := range resultStructs {
			// 結果を確認
			for key, value := range valueList[j] {
				switch key {
				case "id":
					if resultStruct.Id != value.(string) {
						t.Fatalf("%s is invalid\nexpect: \n%s \nactual: \n%s", key, value, resultStruct.Id)
					}
				case "a":
					if resultStruct.A != value.(int) {
						t.Fatalf("%s is invalid\nexpect: \n%d \nactual: \n%d", key, value, resultStruct.A)
					}
				case "b":
					if resultStruct.B != value.(int) {
						t.Fatalf("%s is invalid\nexpect: \n%d \nactual: \n%d", key, value, resultStruct.B)
					}
				case "c":
					if resultStruct.C != value.(int) {
						t.Fatalf("%s is invalid\nexpect: \n%d \nactual: \n%d", key, value, resultStruct.C)
					}
				case "p":
					if resultStruct.P != value.(uint16) {
						t.Fatalf("%s is invalid\nexpect: \n%d \nactual: \n%d", key, value, resultStruct.P)
					}
				default:
					t.Fatalf("ハンドリング漏れ")
				}
			}
			j++
		}
	}
}

func testExecuteQueryInParallel(t *testing.T, tripleN int, forN int, N int) {
	t.Helper()
	funcName := fmt.Sprintf("TestExecuteQueryInParallel%d-%d-%d", tripleN, forN, N)

	host, bucket := testOpts[0][0], testOpts[0][1]
	// 初期化処理
	err := executeInit(host, bucket, funcName)
	if err != nil {
		t.Fatal(err)
	}
	// 今回のテストで使用するクエリ群
	insertQueriesList := make([][]string, N)
	valueList := make([][]map[string]interface{}, N)
	for n := 0; n < N; n++ {
		valueList[n] = []map[string]interface{}{}
		insertQueriesList[n] = make([]string, forN)
	}

	f := getCas()
	for i := 0; i < forN; i++ {
		for n := 0; n < N; n++ {
			values := make([]string, tripleN)
			for j := 0; j < tripleN; j++ {
				// Insertする値
				cas := f()
				partitionId := uint16(cas) & uint16((2<<15)-1)
				valueMap := map[string]interface{}{
					"test_id": funcName,
					"a":       rand.Intn(100000),
					"b":       rand.Intn(100000),
					"c":       rand.Intn(100000),
					"p":       partitionId,
				}
				// Mapを文字列に変換
				valueMapByte, _ := json.Marshal(valueMap)
				value := string(valueMapByte)
				convertedCas := strconv.FormatUint(cas, 10)
				values[j] = fmt.Sprintf("(\"%s\",%s)", convertedCas, value)

				tempValueMap := map[string]interface{}{
					"a":  valueMap["a"],
					"b":  valueMap["b"],
					"c":  valueMap["c"],
					"p":  partitionId,
					"id": convertedCas,
				}
				valueList[partitionId%uint16(N)] = append(valueList[partitionId%uint16(N)], tempValueMap)
			}
			insertQueriesList[n][i] = fmt.Sprintf("INSERT INTO `%s` (KEY, VALUE) VALUES %s;", bucket, strings.Join(values, ", "))
		}
	}

	t.Run("TestExecuteQueryInParallel_Insert", func(t *testing.T) {
		t.Run("TestExecuteQueryInParallel_Insert_Sub0", func(t *testing.T) { t.Parallel(); executeInsertQueryForParallel(t, host, insertQueriesList[0]) })
		t.Run("TestExecuteQueryInParallel_Insert_Sub1", func(t *testing.T) { t.Parallel(); executeInsertQueryForParallel(t, host, insertQueriesList[1]) })
		t.Run("TestExecuteQueryInParallel_Insert_Sub2", func(t *testing.T) { t.Parallel(); executeInsertQueryForParallel(t, host, insertQueriesList[2]) })
		t.Run("TestExecuteQueryInParallel_Insert_Sub3", func(t *testing.T) { t.Parallel(); executeInsertQueryForParallel(t, host, insertQueriesList[3]) })
		t.Run("TestExecuteQueryInParallel_Insert_Sub4", func(t *testing.T) { t.Parallel(); executeInsertQueryForParallel(t, host, insertQueriesList[4]) })
		t.Run("TestExecuteQueryInParallel_Insert_Sub5", func(t *testing.T) { t.Parallel(); executeInsertQueryForParallel(t, host, insertQueriesList[5]) })
		t.Run("TestExecuteQueryInParallel_Insert_Sub6", func(t *testing.T) { t.Parallel(); executeInsertQueryForParallel(t, host, insertQueriesList[6]) })
		t.Run("TestExecuteQueryInParallel_Insert_Sub7", func(t *testing.T) { t.Parallel(); executeInsertQueryForParallel(t, host, insertQueriesList[7]) })
		t.Run("TestExecuteQueryInParallel_Insert_Sub8", func(t *testing.T) { t.Parallel(); executeInsertQueryForParallel(t, host, insertQueriesList[8]) })
		t.Run("TestExecuteQueryInParallel_Insert_Sub9", func(t *testing.T) { t.Parallel(); executeInsertQueryForParallel(t, host, insertQueriesList[9]) })
	})

	t.Run("TestExecuteQueryInParallel_Delete", func(t *testing.T) {
		query0 := fmt.Sprintf("DELETE FROM `%s` x WHERE x.test_id = \"%s\" AND MOD(x.p, %d) = %d LIMIT %d RETURNING meta(x).id, x.a, x.b, x.c, x.p", bucket, funcName, N, 0, tripleN)
		t.Run("TestExecuteQueryInParallel_Delete_Sub0", func(t *testing.T) {
			t.Parallel()
			executeDeleteQueryForParallel(t, host, query0, valueList[0], forN, N, 0)
		})
		query1 := fmt.Sprintf("DELETE FROM `%s` x WHERE x.test_id = \"%s\" AND MOD(x.p, %d) = %d LIMIT %d RETURNING meta(x).id, x.a, x.b, x.c, x.p", bucket, funcName, N, 1, tripleN)
		t.Run("TestExecuteQueryInParallel_Delete_Sub1", func(t *testing.T) {
			t.Parallel()
			executeDeleteQueryForParallel(t, host, query1, valueList[1], forN, N, 1)
		})
		query2 := fmt.Sprintf("DELETE FROM `%s` x WHERE x.test_id = \"%s\" AND MOD(x.p, %d) = %d LIMIT %d RETURNING meta(x).id, x.a, x.b, x.c, x.p", bucket, funcName, N, 2, tripleN)
		t.Run("TestExecuteQueryInParallel_Delete_Sub2", func(t *testing.T) {
			t.Parallel()
			executeDeleteQueryForParallel(t, host, query2, valueList[2], forN, N, 2)
		})
		query3 := fmt.Sprintf("DELETE FROM `%s` x WHERE x.test_id = \"%s\" AND MOD(x.p, %d) = %d LIMIT %d RETURNING meta(x).id, x.a, x.b, x.c, x.p", bucket, funcName, N, 3, tripleN)
		t.Run("TestExecuteQueryInParallel_Delete_Sub3", func(t *testing.T) {
			t.Parallel()
			executeDeleteQueryForParallel(t, host, query3, valueList[3], forN, N, 3)
		})
		query4 := fmt.Sprintf("DELETE FROM `%s` x WHERE x.test_id = \"%s\" AND MOD(x.p, %d) = %d LIMIT %d RETURNING meta(x).id, x.a, x.b, x.c, x.p", bucket, funcName, N, 4, tripleN)
		t.Run("TestExecuteQueryInParallel_Delete_Sub4", func(t *testing.T) {
			t.Parallel()
			executeDeleteQueryForParallel(t, host, query4, valueList[4], forN, N, 4)
		})
		query5 := fmt.Sprintf("DELETE FROM `%s` x WHERE x.test_id = \"%s\" AND MOD(x.p, %d) = %d LIMIT %d RETURNING meta(x).id, x.a, x.b, x.c, x.p", bucket, funcName, N, 5, tripleN)
		t.Run("TestExecuteQueryInParallel_Delete_Sub5", func(t *testing.T) {
			t.Parallel()
			executeDeleteQueryForParallel(t, host, query5, valueList[5], forN, N, 5)
		})
		query6 := fmt.Sprintf("DELETE FROM `%s` x WHERE x.test_id = \"%s\" AND MOD(x.p, %d) = %d LIMIT %d RETURNING meta(x).id, x.a, x.b, x.c, x.p", bucket, funcName, N, 6, tripleN)
		t.Run("TestExecuteQueryInParallel_Delete_Sub6", func(t *testing.T) {
			t.Parallel()
			executeDeleteQueryForParallel(t, host, query6, valueList[6], forN, N, 6)
		})
		query7 := fmt.Sprintf("DELETE FROM `%s` x WHERE x.test_id = \"%s\" AND MOD(x.p, %d) = %d LIMIT %d RETURNING meta(x).id, x.a, x.b, x.c, x.p", bucket, funcName, N, 7, tripleN)
		t.Run("TestExecuteQueryInParallel_Delete_Sub7", func(t *testing.T) {
			t.Parallel()
			executeDeleteQueryForParallel(t, host, query7, valueList[7], forN, N, 7)
		})
		query8 := fmt.Sprintf("DELETE FROM `%s` x WHERE x.test_id = \"%s\" AND MOD(x.p, %d) = %d LIMIT %d RETURNING meta(x).id, x.a, x.b, x.c, x.p", bucket, funcName, N, 8, tripleN)
		t.Run("TestExecuteQueryInParallel_Delete_Sub8", func(t *testing.T) {
			t.Parallel()
			executeDeleteQueryForParallel(t, host, query8, valueList[8], forN, N, 8)
		})
		query9 := fmt.Sprintf("DELETE FROM `%s` x WHERE x.test_id = \"%s\" AND MOD(x.p, %d) = %d LIMIT %d RETURNING meta(x).id, x.a, x.b, x.c, x.p", bucket, funcName, N, 9, tripleN)
		t.Run("TestExecuteQueryInParallel_Delete_Sub9", func(t *testing.T) {
			t.Parallel()
			executeDeleteQueryForParallel(t, host, query9, valueList[9], forN, N, 9)
		})
	})
	// 初期化処理
	err = executeInit(host, bucket, funcName)
	if err != nil {
		t.Fatal(err)
	}

}

func TestExecuteQuery1_100(t *testing.T)              { testExecuteQuery(t, 1, 100) }
func TestExecuteQuery250_4(t *testing.T)              { testExecuteQuery(t, 250, 4) }
func TestExecuteQuery1000_1(t *testing.T)             { testExecuteQuery(t, 1000, 1) }
func TestExecuteQueryInParallel1_10_10(t *testing.T)  { testExecuteQueryInParallel(t, 1, 10, 10) }
func TestExecuteQueryInParallel250_4_10(t *testing.T) { testExecuteQueryInParallel(t, 250, 4, 10) }
func TestExecuteQueryInParalle1000_1_10(t *testing.T) { testExecuteQueryInParallel(t, 1000, 1, 10) }
