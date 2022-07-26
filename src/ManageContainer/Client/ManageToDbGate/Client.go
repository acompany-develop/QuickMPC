package mng2dbgate

import (
	"context"
	"encoding/json"
	"errors"
	"fmt"
	"io"
	"sort"
	"strconv"
	"strings"

	utils "github.com/acompany-develop/QuickMPC/src/ManageContainer/Utils"
	pb "github.com/acompany-develop/QuickMPC/src/Proto/AnyToDbGate"
	pb_types "github.com/acompany-develop/QuickMPC/src/Proto/common_types"
	"google.golang.org/grpc"
)

// 同一IDに対する同時処理を防ぐためのもの
var ls = utils.NewLockSet()

type MetaResult struct {
	PieceID int32 `json:"piece_id"`
}
type ComputationResult struct {
	ID      string     `json:"id"`
	JobUUID string     `json:"job_uuid"`
	Status  int32      `json:"status"`
	Result  string     `json:"result"`
	Meta    MetaResult `json:"meta"`
}

type Client struct{}
type M2DbClient interface {
	InsertShares(string, []string, int32, string, string) error
	DeleteShares([]string) error
	GetSchema(string) ([]string, error)
	GetComputationResult(string) ([]*ComputationResult, error)
	InsertModelParams(string, string, int32) error
	GetDataList() (string, error)
}

// DBGへのconnecterを得る
func connect() (*grpc.ClientConn, error) {
	config, err := utils.GetConfig()
	DbgIP := config.Containers.Dbg
	if err != nil {
		return nil, err
	}
	conn, err := grpc.Dial(DbgIP.Host, grpc.WithInsecure())
	if err != nil {
		return nil, fmt.Errorf("did not connect: %v", err)
	}
	return conn, nil
}

// (conn)にクエリリクエストを送る
func ExecuteQuery(conn *grpc.ClientConn, query string) (string, error) {
	mcToDbGateClient := pb.NewAnyToDbGateClient(conn)
	executeQueryRequest := &pb.ExecuteQueryRequest{Host: "sharedb", Query: query}

	stream, err := mcToDbGateClient.ExecuteQuery(context.TODO(), executeQueryRequest)
	if err != nil {
		return "", nil
	}
	piece_list := map[int]string{}
	for {
		reply, err := stream.Recv()
		if err == io.EOF {
			break
		}
		if err != nil {
			return "", err
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
	return res, nil
}

type Meta struct {
	Schema  []string `json:"schema"`
	PieceID int32    `json:"piece_id"`
}

type Share struct {
	DataID string      `json:"data_id"`
	Meta   Meta        `json:"meta"`
	Value  interface{} `json:"value"`
	SentAt string      `json:"sent_at"`
}

// DBGから key=id で登録されたdata数を取得する
func (c Client) Count(key string, id string, bucket string) (int32, error) {
	conn, err := connect()
	if err != nil {
		return -1, err
	}
	defer conn.Close()
	return c.count(conn, key, id, bucket)
}

// (conn)から key=id で登録されたdata数を取得する
func (c Client) count(conn *grpc.ClientConn, key string, id string, bucket string) (int32, error) {
	ls.Lock(id)
	defer ls.Unlock(id)
	query := fmt.Sprintf("SELECT x.meta, meta().id FROM `%s` x WHERE x.%s= '%s'", bucket, key, id)
	res, err := ExecuteQuery(conn, query)
	if err != nil {
		return -1, err
	}
	var data_json []interface{}
	err = json.Unmarshal([]byte(res), &data_json)
	if err != nil {
		return -1, err
	}
	return int32(len(data_json)), nil
}

// DBGにシェアを送信する
func (c Client) InsertShares(dataID string, schema []string, pieceID int32, shares string, sent_at string) error {
	conn, err := connect()
	if err != nil {
		return err
	}
	defer conn.Close()
	return c.insertShares(conn, dataID, schema, pieceID, shares, sent_at)
}

func (c Client) existPieceID(conn *grpc.ClientConn, dataID string, pieceID int32) (bool, error) {
	ls.Lock(dataID)
	defer ls.Unlock(dataID)
	query := fmt.Sprintf("SELECT x.meta, meta().id FROM `share` x WHERE x.data_id = '%s';", dataID)
	resJson, err := ExecuteQuery(conn, query)

	var res []Share
	err = json.Unmarshal([]byte(resJson), &res)
	if err != nil {
		return false, err
	}
	for _, r := range res {
		if r.Meta.PieceID == pieceID {
			return true, nil
		}
	}
	return false, nil
}

// (conn)にシェアを送信する
func (c Client) insertShares(conn *grpc.ClientConn, dataID string, schema []string, pieceID int32, shares string, sent_at string) error {
	// 重複チェック
	exist, err := c.existPieceID(conn, dataID, pieceID)
	if err != nil {
		return err
	}
	if exist {
		return errors.New("重複データ登録エラー: " + dataID + "は既に登録されています．")
	}

	// シェア送信
	ls.Lock(dataID)
	defer ls.Unlock(dataID)
	var shares_json interface{}
	err = json.Unmarshal([]byte(shares), &shares_json)
	if err != nil {
		return err
	}

	meta := Meta{
		Schema:  schema,
		PieceID: pieceID,
	}
	share := Share{
		DataID: dataID,
		Meta:   meta,
		Value:  shares_json,
		SentAt: sent_at,
	}
	bytes, err := json.Marshal(share)
	if err != nil {
		return err
	}

	query := fmt.Sprintf("INSERT INTO `share` (KEY, VALUE) VALUES (UUID(), %s) RETURNING *;", string(bytes))

	_, err = ExecuteQuery(conn, query)
	return err
}

type DataIdResponse struct {
	DataID string `json:"data_id"`
}

// DBGにシェア削除リクエストを送る
func (c Client) DeleteShares(dataIDs []string) error {
	conn, err := connect()
	if err != nil {
		return err
	}
	defer conn.Close()
	return c.deleteShares(conn, dataIDs)
}

// (conn)にシェア削除リクエストを送信する
func (c Client) deleteShares(conn *grpc.ClientConn, dataIDs []string) error {
	for _, dataID := range dataIDs {
		ls.Lock(dataID)
		defer ls.Unlock(dataID)
	}
	for _, v := range dataIDs {
		v = utils.EscapeInjection(v, utils.Where)
	}
	str_dataIDs := "['" + strings.Join(dataIDs, "', '") + "']"
	squery := fmt.Sprintf("SELECT x.data_id FROM `share` x WHERE x.data_id IN %s;", str_dataIDs)
	response, err := ExecuteQuery(conn, squery)
	if err != nil {
		return err
	}

	var res []DataIdResponse
	err = json.Unmarshal([]byte(response), &res)
	if err != nil {
		return err
	}

	cnt := 0

	for _, v := range dataIDs {
		for _, e := range res {
			if v == e.DataID {
				cnt++
				break
			}
		}
	}

	if len(dataIDs) != cnt {
		return errors.New("存在しないデータIDが含まれています.")
	}

	dquery := fmt.Sprintf("DELETE FROM `share` x WHERE x.data_id IN %s;", str_dataIDs)
	_, err = ExecuteQuery(conn, dquery)
	if err != nil {
		return err
	}

	return nil
}

type MetaResponse struct {
	ID   string `json:"id"`
	Meta Meta   `json:"meta"`
}

// DBGからschemaを得る
func (c Client) GetSchema(dataID string) ([]string, error) {
	conn, err := connect()
	if err != nil {
		return []string{}, err
	}
	defer conn.Close()
	return c.getSchema(conn, dataID)
}

// (conn)からschemaを得る
func (c Client) getSchema(conn *grpc.ClientConn, dataID string) ([]string, error) {
	ls.Lock(dataID)
	defer ls.Unlock(dataID)
	query := fmt.Sprintf("SELECT x.meta, meta().id FROM `share` x WHERE x.data_id = '%s' ORDER BY meta(s).id LIMIT 1;",
		utils.EscapeInjection(dataID, utils.Where))
	response, err := ExecuteQuery(conn, query)
	if err != nil {
		return []string{}, err
	}

	var res []MetaResponse
	err = json.Unmarshal([]byte(response), &res)
	if err != nil {
		return []string{}, err
	}

	if len(res) == 0 {
		return []string{}, errors.New("schema could not be found.")
	}

	return res[0].Meta.Schema, nil
}

// DBGから計算結果を得る
func (c Client) GetComputationResult(jobUUID string) ([]*ComputationResult, error) {
	conn, err := connect()
	if err != nil {
		return nil, err
	}
	defer conn.Close()
	return c.getComputationResult(conn, jobUUID)
}

// (conn)から計算結果を得る
func (c Client) getComputationResult(conn *grpc.ClientConn, jobUUID string) ([]*ComputationResult, error) {
	ls.Lock(jobUUID)
	defer ls.Unlock(jobUUID)
	// TODO: dbのスキーマを変更
	query := fmt.Sprintf("SELECT x.*, meta().id FROM `result` x WHERE x.job_uuid = '%s';",
		utils.EscapeInjection(jobUUID, utils.Where))
	response, err := ExecuteQuery(conn, query)
	if err != nil {
		return nil, err
	}

	var computationResults []*ComputationResult
	err = json.Unmarshal([]byte(response), &computationResults)
	if err != nil {
		return nil, err
	}

	if len(computationResults) == 0 {
		return nil, errors.New("unique computation result could not be found: " + strconv.Itoa(len(computationResults)))
	}

	return computationResults, nil
}

// DBGにモデルパラメータを送信する
func (c Client) InsertModelParams(jobUUID string, params string, pieceId int32) error {
	conn, err := connect()
	if err != nil {
		return err
	}
	defer conn.Close()
	return c.insertModelParams(conn, jobUUID, params, pieceId)
}

// (conn)にモデルパラメータを送信する
func (c Client) insertModelParams(conn *grpc.ClientConn, jobUUID string, params string, pieceId int32) error {
	cnt, err := c.count(conn, "job_uuid", jobUUID, "result")
	ls.Lock(jobUUID)
	defer ls.Unlock(jobUUID)
	if err != nil {
		return err
	}
	if cnt > 0 {
		return errors.New("重複データ登録エラー: " + jobUUID + "は既に登録されています．")
	}

	saveParams := ComputationResult{
		JobUUID: jobUUID,
		Meta: MetaResult{
			PieceID: pieceId,
		},
		Result: params,
		Status: int32(pb_types.JobStatus_COMPLETED),
	}

	bytes, err := json.Marshal(saveParams)
	if err != nil {
		return err
	}

	query := fmt.Sprintf("INSERT INTO `result` (KEY, VALUE) VALUES (UUID(), %s) RETURNING *;", string(bytes))

	_, err = ExecuteQuery(conn, query)
	return err
}

type GetDataListResult struct {
	DataID string `json:"data_id"`
	SentAt string `json:"sent_at"`
}

func (c Client) GetDataList() (string, error) {
	conn, err := connect()
	if err != nil {
		return "", err
	}
	defer conn.Close()
	return c.getDataList(conn)
}

func (c Client) getDataList(conn *grpc.ClientConn) (string, error) {
	query := "SELECT x.data_id, x.sent_at FROM `share` x;"
	result, err := ExecuteQuery(conn, query)
	if err != nil {
		return "", err
	}
	var res []GetDataListResult
	err = json.Unmarshal([]byte(result), &res)
	if err != nil {
		return "", err
	}
	return result, err
}
