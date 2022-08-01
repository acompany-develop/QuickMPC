package a2dbgserver

import (
	"context"
	"encoding/json"
	"fmt"
	"io"
	"math/rand"
	"net"
	"sort"
	"testing"

	"google.golang.org/grpc"
	"google.golang.org/grpc/test/bufconn"

	. "github.com/acompany-develop/QuickMPC/src/DbContainer/DbGate/Log"
	utils "github.com/acompany-develop/QuickMPC/src/DbContainer/DbGate/Utils"
	pb "github.com/acompany-develop/QuickMPC/src/Proto/AnyToDbGate"
)

const bufSize = 1024 * 1024

var (
	hostShareDb string     = "sharedb" // jsonのkeyは全て小文字
	testOpts    [][]string = [][]string{{hostShareDb, "share"}}
	lis         *bufconn.Listener
)

func init() {
	lis = bufconn.Listen(bufSize)
	s := grpc.NewServer()
	pb.RegisterAnyToDbGateServer(s, &server{})

	AppLogger.Info("a2dbg Test Server listening")
	go func() {
		if err := s.Serve(lis); err != nil {
			AppLogger.Error("Test Start 4")
			AppLogger.Fatal(err)
		}
	}()
}

func bufDialer(ctx context.Context, address string) (net.Conn, error) {
	return lis.Dial()
}

func ExecuteInit(client pb.AnyToDbGateClient, ctx context.Context, host string, bucket string, funcName string) error {

	// 初期化処理
	// 前回Failしているとデータが残りSELECTで必ず落ちるのでDELETEをしておく
	initQuery := fmt.Sprintf("DELETE FROM `%s` x WHERE x.test_id = \"%s\";", bucket, funcName)
	stream, err := client.ExecuteQuery(ctx, &pb.ExecuteQueryRequest{Host: host, Query: initQuery})
	if err != nil {
		return err
	}
	for {
		_, err := stream.Recv()
		if err == io.EOF {
			return nil
		}
		if err != nil {
			return err
		}
	}
	return err

}

func TestExecuteQuery(t *testing.T) {

	funcName := "TestExecuteQuery"

	ctx := context.Background()
	conn, err := grpc.DialContext(ctx, "bufnet", grpc.WithContextDialer(bufDialer), grpc.WithInsecure())
	if err != nil {
		t.Fatal(err)
	}
	defer conn.Close()
	client := pb.NewAnyToDbGateClient(conn)

	for _, hostAndBucket := range testOpts {

		host, bucket := hostAndBucket[0], hostAndBucket[1]

		// 初期化処理
		err := ExecuteInit(client, ctx, host, bucket, funcName)
		if err != nil {
			t.Fatal(err)
		}

		// Insertする値
		valueMap := map[string]string{
			"test_id": funcName,
			"a":       fmt.Sprint(rand.Intn(1000)),
			"b":       fmt.Sprint(rand.Intn(1000)),
			"c":       fmt.Sprint(rand.Intn(1000)),
		}
		// Mapを文字列に変換
		valueMapByte, _ := json.Marshal(valueMap)
		valueMapString := string(valueMapByte)

		// 今回のテストで使用するクエリ群
		var queriesList []string = []string{
			fmt.Sprintf("INSERT INTO `%s` (KEY, VALUE) VALUES (UUID(), %s) RETURNING *;", bucket, valueMapString),
			fmt.Sprintf("SELECT x.*, meta().id FROM `%s` x WHERE x.test_id = \"%s\";", bucket, funcName),
			fmt.Sprintf("DELETE FROM `%s` x WHERE x.test_id = \"%s\" RETURNING x.*, meta().id ;", bucket, funcName),
		}

		for _, query := range queriesList {

			// Query実行
			stream, err := client.ExecuteQuery(ctx, &pb.ExecuteQueryRequest{Host: host, Query: query})
			if err != nil {
				t.Fatal(err)
			}

			piece_list := map[int]string{}
			for {
				reply, err := stream.Recv()
				if err == io.EOF {
					break
				}
				if err != nil {
					t.Fatal(err)
				}

				piece_id := int(reply.PieceId)
				result := reply.Result
				piece_list[piece_id] = result
			}

			// piece_idの辞書順毎に値を得られるようにする
			keys := make([]int, len(piece_list), len(piece_list))

			i := 0
			for key := range piece_list {
				keys[i] = int(key)
				i++
			}

			sort.Ints(keys)

			var res string

			for _, key := range keys {
				res += piece_list[key]
			}

			// 結果ログ
			t.Log(res)

			// 結果を"[結果]"の形で受け取るのでSliceへ変更
			resultMapSlice := []map[string]interface{}{}
			err = utils.ConvertJsonStringToStruct(res, &resultMapSlice)
			if err != nil {
				t.Fatal(err)
			}

			// 今回のクエリは1つのTripleしか返ってこないので0でok
			resultMap := resultMapSlice[0]

			// Insertの場合は [{"share":{"a":?,"b":?,"c":?,"test_id":"TestExecuteQuery"}}]
			// 他の場合は [{"a":?,"b":?,"c":?,"test_id":"TestExecuteQuery"}]
			if query[:6] == "INSERT" {
				resultMap = resultMap[bucket].(map[string]interface{})
			}

			// 結果を確認
			for key, value := range valueMap {
				if resultMap[key] != value {
					t.Fatal(fmt.Sprintf("%s returns wrong %s item", query[:6], key))
				}
			}
		}
	}
}
