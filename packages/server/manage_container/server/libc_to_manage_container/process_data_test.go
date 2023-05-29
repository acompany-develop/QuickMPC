package l2mserver

import (
	"encoding/json"
	"fmt"
	"reflect"
	"testing"

	m2db "github.com/acompany-develop/QuickMPC/packages/server/manage_container/client/manage_to_db"
	pb_types "github.com/acompany-develop/QuickMPC/proto/common_types"
)

// Test用のDB
type processDataDb struct{}

var db = map[string]map[int32]m2db.Share{}

func (processDataDb) GetSharePieceSize(dataID string) (int32, error) {
	mp, _ := db[dataID]
	return int32(len(mp)), nil
}
func (processDataDb) InsertShares(dataID string, schema []*pb_types.Schema, pieceID int32, shares string, sentAt string, matchingColumn int32) error {
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
	fmt.Println(share)
	_, ok := db[dataID]
	if !ok {
		db[dataID] = map[int32]m2db.Share{}
	}
	db[dataID][pieceID] = share
	return nil
}
func (processDataDb) DeleteShares([]string) error {
	return nil
}
func (processDataDb) GetSharePiece(dataID string, pieceID int32) (m2db.Share, error) {
	mp, _ := db[dataID]
	share, ok := mp[pieceID]
	if !ok {
		return m2db.Share{}, fmt.Errorf("share not found")
	}
	return share, nil
}
func (processDataDb) GetSchema(string) ([]*pb_types.Schema, error) {
	return []*pb_types.Schema{{Name: "attr1"}}, nil
}
func (processDataDb) GetJobErrorInfo(string) (*pb_types.JobErrorInfo, error) {
	return &pb_types.JobErrorInfo{What: "test"}, nil
}
func (processDataDb) GetComputationResult(string, []string) ([]*m2db.ComputationResult, *pb_types.JobErrorInfo, error) {
	return []*m2db.ComputationResult{{Result: []string{"result"}}, {Result: []string{"result"}}}, nil, nil
}
func (processDataDb) CreateStatusFile(string) error {
	return nil
}
func (processDataDb) DeleteStatusFile(string) error {
	return nil
}
func (processDataDb) GetDataList() (string, error) {
	return "result", nil
}
func (processDataDb) GetElapsedTime(string) (float64, error) {
	return 0, nil
}
func (p processDataDb) GetMatchingColumn(dataID string) (int32, error) {
	share, err := p.GetSharePiece(dataID, 0)
	if err != nil {
		return 0, err
	}
	return share.Meta.MatchingColumn, nil
}

// ID列に加算が正しくできるかテスト
func TestAddValueToCol(t *testing.T) {
	testcases := map[string]struct {
		dataID   string
		data     m2db.Share
		value    []string
		expected m2db.Share
	}{
		"Column1": {
			dataID: "TestAddValueToColColumn1",
			data: m2db.Share{Value: [][]string{{"1", "2"}, {"3", "4"}},
				Meta: m2db.ShareMeta{PieceID: 0, MatchingColumn: 1}},
			value: []string{"10", "100"},
			expected: m2db.Share{Value: [][]string{{"11.00000000000000000000000000000000000000000000000000", "2"}, {"103.00000000000000000000000000000000000000000000000000", "4"}},
				Meta: m2db.ShareMeta{PieceID: 0, MatchingColumn: 1}},
		},
		"Column2": {
			dataID: "TestAddValueToColColumn2",
			data: m2db.Share{Value: [][]string{{"1", "2"}, {"3", "4"}},
				Meta: m2db.ShareMeta{PieceID: 0, MatchingColumn: 2}},
			value: []string{"10", "100"},
			expected: m2db.Share{Value: [][]string{{"1", "12.00000000000000000000000000000000000000000000000000"}, {"3", "104.00000000000000000000000000000000000000000000000000"}},
				Meta: m2db.ShareMeta{PieceID: 0, MatchingColumn: 2}},
		},
	}
	for name, tt := range testcases {
		tt := tt
		t.Run(name, func(t *testing.T) {
			t.Parallel()

			// テスト用データをテスト用DBに保存しておく
			p := processer{m2dbclient: processDataDb{}}
			db[tt.dataID] = map[int32]m2db.Share{}
			db[tt.dataID][0] = tt.data

			// 加算処理をしてDBに加算された値が保存されているか確認
			errAdd := p.addValueToIDCol(tt.dataID, tt.value)
			if errAdd != nil {
				t.Error(errAdd)
			}
			share, errShare := p.m2dbclient.GetSharePiece(tt.dataID, 0)
			if errShare != nil {
				t.Error(errShare)
			}
			if !reflect.DeepEqual(share, tt.expected) {
				t.Errorf("share must be %v, but %v", tt.expected, share)
			}
		})
	}
}

// piece分割されたShareのID列に加算が正しくできるかテスト
func TestAddValueToColPiece(t *testing.T) {
	testcases := map[string]struct {
		dataID   string
		data     []m2db.Share
		value    []string
		expected []m2db.Share
	}{
		"Column1": {
			dataID: "TestAddValueToColPieceColumn1",
			data: []m2db.Share{
				{Value: [][]string{{"1", "2"}}, Meta: m2db.ShareMeta{PieceID: 0, MatchingColumn: 1}},
				{Value: [][]string{{"3", "4"}}, Meta: m2db.ShareMeta{PieceID: 0, MatchingColumn: 1}},
				{Value: [][]string{{"5", "6"}}, Meta: m2db.ShareMeta{PieceID: 0, MatchingColumn: 1}},
			},
			value: []string{"10", "100", "1000"},
			expected: []m2db.Share{
				{Value: [][]string{{"11.00000000000000000000000000000000000000000000000000", "2"}}, Meta: m2db.ShareMeta{PieceID: 0, MatchingColumn: 1}},
				{Value: [][]string{{"103.00000000000000000000000000000000000000000000000000", "4"}}, Meta: m2db.ShareMeta{PieceID: 0, MatchingColumn: 1}},
				{Value: [][]string{{"1005.00000000000000000000000000000000000000000000000000", "6"}}, Meta: m2db.ShareMeta{PieceID: 0, MatchingColumn: 1}},
			},
		},
		"Column2": {
			dataID: "TestAddValueToColPieceColumn2",
			data: []m2db.Share{
				{Value: [][]string{{"1", "2"}}, Meta: m2db.ShareMeta{PieceID: 0, MatchingColumn: 2}},
				{Value: [][]string{{"3", "4"}}, Meta: m2db.ShareMeta{PieceID: 0, MatchingColumn: 2}},
				{Value: [][]string{{"5", "6"}}, Meta: m2db.ShareMeta{PieceID: 0, MatchingColumn: 2}},
			},
			value: []string{"10", "100", "1000"},
			expected: []m2db.Share{
				{Value: [][]string{{"1", "12.00000000000000000000000000000000000000000000000000"}}, Meta: m2db.ShareMeta{PieceID: 0, MatchingColumn: 2}},
				{Value: [][]string{{"3", "104.00000000000000000000000000000000000000000000000000"}}, Meta: m2db.ShareMeta{PieceID: 0, MatchingColumn: 2}},
				{Value: [][]string{{"5", "1006.00000000000000000000000000000000000000000000000000"}}, Meta: m2db.ShareMeta{PieceID: 0, MatchingColumn: 2}},
			},
		},
	}
	for name, tt := range testcases {
		tt := tt
		t.Run(name, func(t *testing.T) {
			t.Parallel()

			// テスト用データを1行ずつテスト用DBに保存しておく
			p := processer{m2dbclient: processDataDb{}}
			db[tt.dataID] = map[int32]m2db.Share{}
			for pieceID, share := range tt.data {
				db[tt.dataID][int32(pieceID)] = share
			}

			// 加算処理をしてDBに加算された値が保存されているか確認
			errAdd := p.addValueToIDCol(tt.dataID, tt.value)
			if errAdd != nil {
				t.Error(errAdd)
			}
			for pieceID := 0; pieceID < len(tt.data); pieceID++ {
				share, errShare := p.m2dbclient.GetSharePiece(tt.dataID, int32(pieceID))
				if errShare != nil {
					t.Error(errShare)
				}
				if !reflect.DeepEqual(share, tt.expected[pieceID]) {
					t.Errorf("share must be %v, but %v", tt.expected[pieceID], share)
				}
			}

		})
	}
}
