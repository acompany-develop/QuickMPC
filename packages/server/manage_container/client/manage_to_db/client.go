package mng2db

import (
	"encoding/json"
	"errors"
	"fmt"
	"io/ioutil"
	"os"
	"path/filepath"
	"strconv"

	"google.golang.org/protobuf/encoding/protojson"

	. "github.com/acompany-develop/QuickMPC/packages/server/manage_container/log"
	utils "github.com/acompany-develop/QuickMPC/packages/server/manage_container/utils"
	pb_types "github.com/acompany-develop/QuickMPC/proto/common_types"
)

// DBの役割を果たすディレクトリのパス
const shareDbPath = "/db/share"
const resultDbPath = "/db/result"

// 同一IDに対する同時処理を防ぐためのもの
var ls = utils.NewLockSet()

// 計算結果形式
type ComputationResultMeta struct {
	ColumnNumber int32 `json:"column_number"`
	PieceID      int32 `json:"piece_id"`
	ResultType   string
}
type ComputationResult struct {
	ID      string                `json:"id"`
	JobUUID string                `json:"job_uuid"`
	Result  []string              `json:"result"`
	Meta    ComputationResultMeta `json:"meta"`
}

// Share形式
type ShareMeta struct {
	Schema         []*pb_types.Schema `json:"schema"`
	PieceID        int32              `json:"piece_id"`
	MatchingColumn int32              `json:"matching_column"`
}
type Share struct {
	DataID string     `json:"data_id"`
	Meta   ShareMeta  `json:"meta"`
	Value  [][]string `json:"value"`
	SentAt string     `json:"sent_at"`
}

// 外部から呼ばれるinterface
type Client struct{}
type M2DbClient interface {
	GetSharePieceSize(string) (int32, error)
	InsertShares(string, []*pb_types.Schema, int32, string, string, int32) error
	DeleteShares([]string) error
	GetSharePiece(string, int32) (Share, error)
	GetSchema(string) ([]*pb_types.Schema, error)
	GetComputationStatus(string) (pb_types.JobStatus, error)
	GetJobErrorInfo(string) *pb_types.JobErrorInfo
	GetComputationResult(string, []string) ([]*ComputationResult, error)
	GetDataList() (string, error)
	GetElapsedTime(string) (float64, error)
	GetMatchingColumn(string) (int32, error)
	CreateStatusFile(string) error
	DeleteStatusFile(string) error
}

// path(ファイル，ディレクトリ)が存在するか
func isExists(path string) bool {
	_, err := os.Stat(path)
	return err == nil
}

func (c Client) GetSharePieceSize(dataID string) (int32, error) {
	path := fmt.Sprintf("%s/%s", shareDbPath, dataID)
	files, err := ioutil.ReadDir(path)
	size := len(files)
	if err != nil || size == 0 {
		return 0, fmt.Errorf("データ未登録エラー: %sは登録されていません．", dataID)
	}
	return int32(size), nil
}

// DBにシェアを保存する
func (c Client) InsertShares(dataID string, schema []*pb_types.Schema, pieceID int32, shares string, sentAt string, matchingColumn int32) error {
	dataPath := fmt.Sprintf("%s/%s/%d", shareDbPath, dataID, pieceID)
	ls.Lock(dataPath)
	defer ls.Unlock(dataPath)

	if isExists(dataPath) {
		return errors.New("重複データ登録エラー: " + dataID + "は既に登録されています．")
	}
	os.Mkdir(fmt.Sprintf("%s/%s", shareDbPath, dataID), 0777)

	var sharesJson [][]string
	errUnmarshal := json.Unmarshal([]byte(shares), &sharesJson)
	if errUnmarshal != nil {
		return errUnmarshal
	}
	share := Share{
		DataID: dataID,
		Meta: ShareMeta{
			Schema:         schema,
			PieceID:        pieceID,
			MatchingColumn: matchingColumn,
		},
		Value:  sharesJson,
		SentAt: sentAt,
	}
	bytes, errMarshal := json.Marshal(share)
	if errMarshal != nil {
		return errMarshal
	}

	errWrite := ioutil.WriteFile(dataPath, bytes, 0777)
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

func (c Client) GetSharePiece(dataID string, pieceID int32) (Share, error) {

	path := fmt.Sprintf("%s/%s/%d", shareDbPath, dataID, 0)
	ls.Lock(path)
	defer ls.Unlock(path)

	if !isExists(path) {
		return Share{}, errors.New("重複データ登録エラー: " + dataID + "は既に登録されています．")
	}

	raw, errRead := ioutil.ReadFile(path)
	if errRead != nil {
		return Share{}, errRead
	}

	var data Share
	errUnmarshal := json.Unmarshal(raw, &data)
	if errUnmarshal != nil {
		return Share{}, errUnmarshal
	}

	return data, nil
}

// DBからschemaを得る
func (c Client) GetSchema(dataID string) ([]*pb_types.Schema, error) {
	data, err := c.GetSharePiece(dataID, 0)
	return data.Meta.Schema, err
}

var (
	errorSpecificStatusFileNotFound = errors.New("specific status file not found")
	errorAnyStatusFileNotFound      = errors.New("any status file not found")
)

func readStatusFile(root string, status pb_types.JobStatus) ([]byte, error) {
	path := fmt.Sprintf("%s/status_%s", root, pb_types.JobStatus_name[int32(status)])
	if isExists(path) {
		return ioutil.ReadFile(path)
	}
	return nil, fmt.Errorf(`file: "%#v" is not found, error: %w`, path, errorSpecificStatusFileNotFound)
}

func (c Client) GetComputationStatus(jobUUID string) (pb_types.JobStatus, error) {
	ls.Lock(jobUUID)
	defer ls.Unlock(jobUUID)

	path := fmt.Sprintf("%s/%s", resultDbPath, jobUUID)
	// NOTE: 降順だとERRORが他のStatusより後ろなので先に判定する
	_, err := readStatusFile(path, pb_types.JobStatus_ERROR)
	if err == nil {
		return pb_types.JobStatus_ERROR, err
	}

	// JobStatus を降順で探す
	statusSize := len(pb_types.JobStatus_value)
	for i := statusSize - 1; i > 0; i-- {
		status := pb_types.JobStatus(i)
		_, err := readStatusFile(path, status)

		if errors.Is(err, errorSpecificStatusFileNotFound) {
			continue
		}

		return status, err
	}

	return pb_types.JobStatus_UNKNOWN, errorAnyStatusFileNotFound
}

func (c Client) GetJobErrorInfo(jobUUID string) *pb_types.JobErrorInfo {
	ls.Lock(jobUUID)
	defer ls.Unlock(jobUUID)

	path := fmt.Sprintf("%s/%s", resultDbPath, jobUUID)

	raw, err := readStatusFile(path, pb_types.JobStatus_ERROR)
	if err != nil {
		return nil
	}
	info := &pb_types.JobErrorInfo{}
	err = protojson.Unmarshal(raw, info)
	return info
}

// DBから計算結果を得る
func (c Client) GetComputationResult(jobUUID string, resultTypes []string) ([]*ComputationResult, error) {
	ls.Lock(jobUUID)
	defer ls.Unlock(jobUUID)

	path := fmt.Sprintf("%s/%s", resultDbPath, jobUUID)
	if !isExists(path + "/completed") {
		// 計算が終了していない場合は空の配列を返す
		return nil, errors.New("unique computation result could not be found")
	}

	var computationResults []*ComputationResult
	for _, resultType := range resultTypes {
		files, _ := filepath.Glob(path + "/" + resultType + "_*")
		for _, piecePath := range files {
			raw, errRead := ioutil.ReadFile(piecePath)
			if errRead != nil {
				return nil, errRead
			}

			var result *ComputationResult
			errUnmarshal := json.Unmarshal(raw, &result)
			if errUnmarshal != nil {
				return nil, errUnmarshal
			}
			result.Meta.ResultType = resultType
			computationResults = append(computationResults, result)
		}
	}

	if len(computationResults) == 0 {
		return nil, errors.New("unique computation result could not be found: " + strconv.Itoa(len(computationResults)))
	}

	return computationResults, nil
}

// DBからdata一覧を取得する
func (c Client) GetDataList() (string, error) {
	AppLogger.Warning("GetDataList()は削除予定の非推奨機能です．")
	// NOTE: 一部のテストで使用しているため削除まではnilを返す
	return "", nil
}

func getTime(path string) (float64, error) {
	raw, errRead := ioutil.ReadFile(path)
	if errRead != nil {
		return 0, errRead
	}
	time, err := strconv.ParseFloat(string(raw), 64)
	if err != nil {
		return 0, err
	}
	return time / 1000, nil
}

func getElapsedTime(path string) (float64, error) {
	start, startErr := getTime(fmt.Sprintf("%s/status_PRE_JOB", path))
	if startErr != nil {
		return 0, startErr
	}
	end, endErr := getTime(fmt.Sprintf("%s/status_COMPLETED", path))
	if endErr != nil {
		return 0, endErr
	}
	return end - start, nil
}

func (c Client) GetElapsedTime(jobUUID string) (float64, error) {
	ls.Lock(jobUUID)
	defer ls.Unlock(jobUUID)

	path := fmt.Sprintf("%s/%s", resultDbPath, jobUUID)
	return getElapsedTime(path)
}

func (c Client) GetMatchingColumn(dataID string) (int32, error) {
	data, err := c.GetSharePiece(dataID, 0)
	return data.Meta.MatchingColumn, err
}

func (c Client) CreateStatusFile(jobUUID string) error {
	ls.Lock(jobUUID)
	defer ls.Unlock(jobUUID)
	path := fmt.Sprintf("%s/%s", resultDbPath, jobUUID)
	err := os.Mkdir(path, 0777)
	if err != nil {
		return err
	}
	fp, err := os.Create(fmt.Sprintf("%s/status_%s", path, pb_types.JobStatus_RECEIVED.String()))
	fp.Close()

	return err
}

func (c Client) DeleteStatusFile(jobUUID string) error {
	ls.Lock(jobUUID)
	defer ls.Unlock(jobUUID)
	path := fmt.Sprintf("%s/%s", resultDbPath, jobUUID)
	return os.RemoveAll(path)
}
