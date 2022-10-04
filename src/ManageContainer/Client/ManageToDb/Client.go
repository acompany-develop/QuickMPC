// 新DB作成が完了したら現存のManageToDbGateと入れ替える
package mng2db

import (
	"encoding/json"
	"errors"
	"fmt"
	"io/ioutil"
	"os"
	"path/filepath"
	"strconv"

	utils "github.com/acompany-develop/QuickMPC/src/ManageContainer/Utils"
)

// 同一IDに対する同時処理を防ぐためのもの
var ls = utils.NewLockSet()

// 計算結果形式
type ComputationResultMeta struct {
	PieceID int32 `json:"piece_id"`
}
type ComputationResult struct {
	ID      string                `json:"id"`
	JobUUID string                `json:"job_uuid"`
	Status  int32                 `json:"status"`
	Result  string                `json:"result"`
	Meta    ComputationResultMeta `json:"meta"`
}

//
type ShareMeta struct {
	Schema  []string `json:"schema"`
	PieceID int32    `json:"piece_id"`
}
type Share struct {
	DataID string      `json:"data_id"`
	Meta   ShareMeta   `json:"meta"`
	Value  interface{} `json:"value"`
	SentAt string      `json:"sent_at"`
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

const shareDbPath = "/Db/share"
const resultDbPath = "/Db/result"

func isExists(path string) bool {
	_, err := os.Stat(path)
	return err == nil
}

// DBにシェアを保存する
func (c Client) InsertShares(dataID string, schema []string, pieceID int32, shares string, sent_at string) error {
	dataPath := fmt.Sprintf("%s/%s/%d", shareDbPath, dataID, pieceID)
	ls.Lock(dataPath)
	defer ls.Unlock(dataPath)

	if isExists(dataPath) {
		return errors.New("重複データ登録エラー: " + dataID + "は既に登録されています．")
	}
	os.Mkdir(fmt.Sprintf("%s/%s", shareDbPath, dataID), 0777)

	var shares_json interface{}
	errUnmarshal := json.Unmarshal([]byte(shares), &shares_json)
	if errUnmarshal != nil {
		return errUnmarshal
	}
	meta := ShareMeta{
		Schema:  schema,
		PieceID: pieceID,
	}
	share := Share{
		DataID: dataID,
		Meta:   meta,
		Value:  shares_json,
		SentAt: sent_at,
	}
	bytes, errMarshal := json.Marshal(share)
	if errMarshal != nil {
		return errMarshal
	}

	errWrite := ioutil.WriteFile(dataPath, bytes, 0666)
	if errWrite != nil {
		return errWrite
	}

	return nil
}

// DBから指定されたシェアを削除する
func (c Client) DeleteShares(dataIDs []string) error {
	for _, dataID := range dataIDs {
		ls.Lock(dataID)
		path := fmt.Sprintf("%s/%s", shareDbPath, dataID)
		if isExists(path) {
			err := os.RemoveAll(path)
			if err != nil {
				return err
			}
		}
		ls.Unlock(dataID)
	}
	return nil
}

// DBからschemaを得る
func (c Client) GetSchema(dataID string) ([]string, error) {

	path := fmt.Sprintf("%s/%s/%d", shareDbPath, dataID, 1)
	ls.Lock(path)
	defer ls.Unlock(path)

	if !isExists(path) {
		errMessage := "データ未登録エラー: " + dataID + "は登録されていません．"
		return []string{}, errors.New(errMessage)
	}

	raw, errRead := ioutil.ReadFile(path)
	if errRead != nil {
		return []string{}, errRead
	}

	var data Share
	errUnmarshal := json.Unmarshal(raw, &data)
	if errUnmarshal != nil {
		return []string{}, errUnmarshal
	}

	return data.Meta.Schema, nil
}

// DBから計算結果を得る
func (c Client) GetComputationResult(jobUUID string) ([]*ComputationResult, error) {
	ls.Lock(jobUUID)
	defer ls.Unlock(jobUUID)

	path := fmt.Sprintf("%s/%s", resultDbPath, jobUUID)
	if !isExists(path + "/completed") {
		// TODO: statusの保存方法を決定して取得する
		// 旧仕様では存在しない場合はstatusだけ返してエラーはnilとしてる
		// statusすら存在しない場合に限りエラーを返す
		status := 0
		errMessage := fmt.Sprintf("computation result is not found: status is %d", status)
		return nil, errors.New(errMessage)
	}

	var computationResults []*ComputationResult
	files, _ := filepath.Glob(path + "/*")
	for _, piecePath := range files {
		if piecePath == path+"/completed" {
			continue
		}
		raw, errRead := ioutil.ReadFile(piecePath)
		if errRead != nil {
			return nil, errRead
		}

		var result *ComputationResult
		errUnmarshal := json.Unmarshal(raw, &result)
		if errUnmarshal != nil {
			return nil, errUnmarshal
		}
		computationResults = append(computationResults, result)
	}

	if len(computationResults) == 0 {
		return nil, errors.New("unique computation result could not be found: " + strconv.Itoa(len(computationResults)))
	}

	return computationResults, nil
}

// DBにモデルパラメータを保存する
func (c Client) InsertModelParams(jobUUID string, params string, pieceId int32) error {
	return nil
}

// DBからdata一覧を取得する
func (c Client) GetDataList() (string, error) {
	return "", nil
}
