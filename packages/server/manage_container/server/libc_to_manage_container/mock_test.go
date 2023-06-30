// test用の各種mockを定義している
package l2mserver

import (
	"encoding/json"
	"errors"
	"fmt"
	"sync"

	m2db "github.com/acompany-develop/QuickMPC/packages/server/manage_container/client/manage_to_db"
	pb_types "github.com/acompany-develop/QuickMPC/proto/common_types"
	pb_m2c "github.com/acompany-develop/QuickMPC/proto/manage_to_computation_container"
)

// Test用の各種mock
type localDb struct{}
type localCC struct{}
type localMC struct{}
type localTokenCA struct{}

/* ---------- DBのmock ---------- */
var mu sync.Mutex
var db = map[string]map[int32]m2db.Share{}

const exist_data_id = "exist_data_id"

func (localDb) GetSharePieceSize(dataID string) (int32, error) {
	if dataID == exist_data_id {
		return 1, nil
	}
	mu.Lock()
	defer mu.Unlock()
	mp, ok := db[dataID]
	if !ok {
		return 0, fmt.Errorf("データ未登録エラー: %sは登録されていません．", dataID)
	}
	return int32(len(mp)), nil
}
func (localDb) InsertShares(dataID string, schema []*pb_types.Schema, pieceID int32, shares string, sentAt string, matchingColumn int32) error {
	var sharesJson [][]string
	errUnmarshal := json.Unmarshal([]byte(shares), &sharesJson)
	if errUnmarshal != nil {
		return errUnmarshal
	}
	share := m2db.Share{
		DataID: dataID,
		Meta: m2db.ShareMeta{
			Schema:         schema,
			PieceID:        pieceID,
			MatchingColumn: matchingColumn,
		},
		Value:  sharesJson,
		SentAt: sentAt,
	}
	mu.Lock()
	defer mu.Unlock()
	_, ok := db[dataID]
	if !ok {
		db[dataID] = map[int32]m2db.Share{}
	}
	if _, ok := db[dataID][pieceID]; ok {
		return errors.New("重複データ登録エラー: " + dataID + "は既に登録されています．")
	}
	db[dataID][pieceID] = share
	return nil
}
func (localDb) DeleteShares([]string) error {
	return nil
}
func (localDb) GetSharePiece(dataID string, pieceID int32) (m2db.Share, error) {
	if dataID == exist_data_id {
		return m2db.Share{Value: [][]string{{"1"}, {"2"}}, Meta: m2db.ShareMeta{Schema: []*pb_types.Schema{{Name: "attr1"}}, MatchingColumn: 1}}, nil
	}
	mu.Lock()
	defer mu.Unlock()
	mp, _ := db[dataID]
	share, ok := mp[pieceID]
	if !ok {
		return m2db.Share{}, fmt.Errorf("share not found")
	}
	// deep copy
	var ret m2db.Share
	b, _ := json.Marshal(share)
	json.Unmarshal(b, &ret)
	return ret, nil
}
func (localDb) GetSchema(string) ([]*pb_types.Schema, error) {
	return []*pb_types.Schema{{Name: "attr1"}}, nil
}
func (localDb) GetJobErrorInfo(string) (*pb_types.JobErrorInfo, error) {
	return &pb_types.JobErrorInfo{What: "test"}, nil
}
func (localDb) GetComputationResult(string, []string) ([]*m2db.ComputationResult, *pb_types.JobErrorInfo, error) {
	return []*m2db.ComputationResult{{Result: []string{"result"}}, {Result: []string{"result"}}}, nil, nil
}
func (localDb) CreateStatusFile(string) error {
	return nil
}
func (localDb) DeleteStatusFile(string) error {
	return nil
}
func (localDb) GetDataList() (string, error) {
	return "result", nil
}
func (localDb) GetElapsedTime(string) (float64, error) {
	return 0, nil
}
func (p localDb) GetMatchingColumn(dataID string) (int32, error) {
	share, err := p.GetSharePiece(dataID, 0)
	if err != nil {
		return 0, err
	}
	return share.Meta.MatchingColumn, nil
}

/* ---------- CCのmock ---------- */
func (localCC) ExecuteComputation(*pb_m2c.ExecuteComputationRequest) (string, int32, error) {
	return "", 0, nil
}
func (localCC) CheckProgress(string) (*pb_types.JobProgress, error) {
	return nil, nil
}

/* ---------- MCのmock ---------- */
func (localMC) DeleteShares(string) error {
	return nil
}
func (localMC) Sync(string) error {
	return nil
}
func (localMC) CreateStatusFile(string) error {
	return nil
}
func (localMC) DeleteStatusFile(string) error {
	return nil
}

/* ---------- TokenCAのmock ---------- */
func (localTokenCA) AuthorizeDep(token string) error {
	return nil
}
func (localTokenCA) AuthorizeDemo(token string) error {
	return nil
}
