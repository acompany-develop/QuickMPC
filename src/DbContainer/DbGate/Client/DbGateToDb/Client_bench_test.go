package dbg2db_test

import (
	"encoding/json"
	"fmt"
	"math/rand"
	"strings"
	"testing"

	"github.com/google/uuid"
)

// TODO: PCの廃止によりtriple系のN1QLはBench不要になったため, 代わりにshare系のN1QLをBenchするように変更
func benchmarkInsert(b *testing.B, numberOfShare int) {
	funcName := fmt.Sprintf("BenchmarkInsert%d", numberOfShare)
	host := hostShareDb
	bucket := "share"

	// Insertする値
	valuesList := make([]string, numberOfShare)
	for i := 0; i < numberOfShare; i++ {
		valueMap := map[string]string{
			"a":       "1",
			"b":       "2",
			"c":       "3",
			"test_id": funcName,
		}
		valueMapByte, _ := json.Marshal(valueMap)
		valueMapString := string(valueMapByte)
		uuidObj, _ := uuid.NewRandom()
		uuidString := uuidObj.String()
		value := fmt.Sprintf("(\"%s\", %s)", uuidString, valueMapString)
		valuesList[i] = value
	}
	// valueを,で結合
	valuesString := strings.Join(valuesList[:], ",")

	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		result, err := insert(host, bucket, funcName, valuesString)
		if err != nil {
			b.Logf("result: %s\nerr: %s", result, err)
		}
	}
	b.StopTimer()

	executeInit(host, bucket, funcName)
}

func benchmarkDelete(b *testing.B, numberOfShare int) {
	funcName := fmt.Sprintf("BenchmarkDelete%d", numberOfShare)
	host := hostShareDb
	bucket := "share"

	// Insertする値
	valuesList := make([]string, numberOfShare)
	for i := 0; i < numberOfShare; i++ {
		valueMap := map[string]string{
			"test_id": funcName,
			"a":       fmt.Sprint(rand.Intn(100000)),
			"b":       fmt.Sprint(rand.Intn(100000)),
			"c":       fmt.Sprint(rand.Intn(100000)),
		}
		valueMapByte, _ := json.Marshal(valueMap)
		valueMapString := string(valueMapByte)
		uuidObj, _ := uuid.NewRandom()
		uuidString := uuidObj.String()
		value := fmt.Sprintf("(\"%s\", %s)", uuidString, valueMapString)
		valuesList[i] = value
	}
	// valueを,で結合
	valuesString := strings.Join(valuesList[:], ",")

	b.ResetTimer()

	for i := 0; i < b.N; i++ {
		b.StopTimer()
		result, err := insert(host, bucket, funcName, valuesString)
		if err != nil {
			b.Logf("result: %s\nerr: %s", result, err)
		}

		b.StartTimer()
		result, err = deleteRet(host, bucket, funcName, numberOfShare)
		if err != nil {
			b.Logf("result: %s\nerr: %s", result, err)
		}
		// result, err = _select(host, bucket, funcName, numberOfShare)
		// if err != nil {
		// 	b.Logf("result: %s\nerr: %s", result, err)
		// }
		// b.StopTimer()
		// result, err = delete(host, bucket, funcName, numberOfShare)
		// if err != nil {
		// 	b.Logf("result: %s\nerr: %s", result, err)
		// }
		// b.StartTimer()
	}
	b.StopTimer()

	executeInit(host, bucket, funcName)
}

func BenchmarkInsert1(b *testing.B) {
	benchmarkInsert(b, 1)
}
func BenchmarkInsert10(b *testing.B) {
	benchmarkInsert(b, 10)
}
func BenchmarkInsert100(b *testing.B) {
	benchmarkInsert(b, 100)
}
func BenchmarkInsert1000(b *testing.B) {
	benchmarkInsert(b, 1000)
}
func BenchmarkInsert10000(b *testing.B) {
	benchmarkInsert(b, 10000)
}

func BenchmarkDelete1(b *testing.B) {
	benchmarkDelete(b, 1)
}
func BenchmarkDelete10(b *testing.B) {
	benchmarkDelete(b, 10)
}
func BenchmarkDelete100(b *testing.B) {
	benchmarkDelete(b, 100)
}
func BenchmarkDelete1000(b *testing.B) {
	benchmarkDelete(b, 1000)
}
func BenchmarkDelete10000(b *testing.B) {
	benchmarkDelete(b, 10000)
}
