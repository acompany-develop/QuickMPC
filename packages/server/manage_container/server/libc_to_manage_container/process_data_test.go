package l2mserver

import (
	"fmt"
	"reflect"
	"testing"

	m2db "github.com/acompany-develop/QuickMPC/packages/server/manage_container/client/manage_to_db"
)

// 加算したテーブルが正しく保存されるかテスト
func TestAddShareDataFrameSave(t *testing.T) {
	testcases := map[string]struct {
		baseData m2db.Share
		addData  m2db.Share
		expected m2db.Share
	}{
		"NormalCase": {
			baseData: m2db.Share{Value: [][]string{{"1", "2"}, {"3", "4"}},
				Meta: m2db.ShareMeta{PieceID: 0, MatchingColumn: 1}},
			addData: m2db.Share{Value: [][]string{{"10", "100"}, {"10", "100"}},
				Meta: m2db.ShareMeta{PieceID: 0, MatchingColumn: 1}},
			expected: m2db.Share{Value: [][]string{
				{"11.00000000000000000000000000000000000000000000000000",
					"102.00000000000000000000000000000000000000000000000000"},
				{"13.00000000000000000000000000000000000000000000000000",
					"104.00000000000000000000000000000000000000000000000000"},
			},
				DataID: "8bf179ec763fd05e429a2ae5847cb47c2a116709e5f5df9b3c9256b278cefe51",
				Meta:   m2db.ShareMeta{PieceID: 0, MatchingColumn: 1}},
		},
	}
	for name, tt := range testcases {
		name := name
		tt := tt
		t.Run(name, func(t *testing.T) {
			t.Parallel()

			// テスト用データをテスト用DBに保存しておく
			dataID1 := fmt.Sprintf("TestAddShareDataFrameSave%s%d", name, 1)
			dataID2 := fmt.Sprintf("TestAddShareDataFrameSave%s%d", name, 2)
			mu.Lock()
			db[dataID1] = map[int32]m2db.Share{}
			db[dataID1][0] = tt.baseData
			db[dataID2] = map[int32]m2db.Share{}
			db[dataID2][0] = tt.addData
			mu.Unlock()

			// 加算処理をしてDBに加算された値が保存されているか確認
			p := processer{m2dbclient: localDb{}}
			dataID, errAdd := p.addShareDataFrame(dataID1, dataID2)
			if errAdd != nil {
				t.Fatal(errAdd)
			}
			share, errShare := p.m2dbclient.GetSharePiece(dataID, 0)
			if errShare != nil {
				t.Fatal(errShare)
			}
			if !reflect.DeepEqual(share, tt.expected) {
				t.Fatalf("share must be %v, but %v", tt.expected, share)
			}
		})
	}
}

// 加算したpiece分割されたテーブルが正しく保存されるかテスト
func TestAddPieceShareDataFrameSave(t *testing.T) {
	testcases := map[string]struct {
		baseData []m2db.Share
		addData  []m2db.Share
		expected []m2db.Share
	}{
		"NormalCase": {
			baseData: []m2db.Share{
				{Value: [][]string{{"1", "2"}}, Meta: m2db.ShareMeta{PieceID: 0, MatchingColumn: 1}},
				{Value: [][]string{{"3", "4"}}, Meta: m2db.ShareMeta{PieceID: 1, MatchingColumn: 1}},
				{Value: [][]string{{"5", "6"}}, Meta: m2db.ShareMeta{PieceID: 2, MatchingColumn: 1}},
			},
			addData: []m2db.Share{
				{Value: [][]string{{"10", "100"}}, Meta: m2db.ShareMeta{PieceID: 0, MatchingColumn: 1}},
				{Value: [][]string{{"10", "100"}}, Meta: m2db.ShareMeta{PieceID: 1, MatchingColumn: 1}},
				{Value: [][]string{{"10", "100"}}, Meta: m2db.ShareMeta{PieceID: 2, MatchingColumn: 1}},
			},
			expected: []m2db.Share{
				{Value: [][]string{{
					"11.00000000000000000000000000000000000000000000000000",
					"102.00000000000000000000000000000000000000000000000000"}},
					DataID: "197425564f8a14fcbbf165a88daa75afc83d8ffb4a2085610b08282349d936ed",
					Meta:   m2db.ShareMeta{PieceID: 0, MatchingColumn: 1}},
				{Value: [][]string{{
					"13.00000000000000000000000000000000000000000000000000",
					"104.00000000000000000000000000000000000000000000000000"}},
					DataID: "197425564f8a14fcbbf165a88daa75afc83d8ffb4a2085610b08282349d936ed",
					Meta:   m2db.ShareMeta{PieceID: 1, MatchingColumn: 1}},
				{Value: [][]string{{
					"15.00000000000000000000000000000000000000000000000000",
					"106.00000000000000000000000000000000000000000000000000"}},
					DataID: "197425564f8a14fcbbf165a88daa75afc83d8ffb4a2085610b08282349d936ed",
					Meta:   m2db.ShareMeta{PieceID: 2, MatchingColumn: 1}},
			},
		},
		"DiffPieceCase1": {
			baseData: []m2db.Share{
				{Value: [][]string{{"1", "2"}}, Meta: m2db.ShareMeta{PieceID: 0, MatchingColumn: 1}},
				{Value: [][]string{{"3", "4"}}, Meta: m2db.ShareMeta{PieceID: 1, MatchingColumn: 1}},
				{Value: [][]string{{"5", "6"}}, Meta: m2db.ShareMeta{PieceID: 2, MatchingColumn: 1}},
			},
			addData: []m2db.Share{
				{Value: [][]string{{"10", "100"}, {"10", "100"}}, Meta: m2db.ShareMeta{PieceID: 0, MatchingColumn: 1}},
				{Value: [][]string{{"10", "100"}}, Meta: m2db.ShareMeta{PieceID: 1, MatchingColumn: 1}},
			},
			expected: []m2db.Share{
				{Value: [][]string{{
					"11.00000000000000000000000000000000000000000000000000",
					"102.00000000000000000000000000000000000000000000000000"}},
					DataID: "7817e5c7355dd7439e28d3af0d7720f128d320f1c9fe6a3837522fff87f68411",
					Meta:   m2db.ShareMeta{PieceID: 0, MatchingColumn: 1}},
				{Value: [][]string{{
					"13.00000000000000000000000000000000000000000000000000",
					"104.00000000000000000000000000000000000000000000000000"}},
					DataID: "7817e5c7355dd7439e28d3af0d7720f128d320f1c9fe6a3837522fff87f68411",
					Meta:   m2db.ShareMeta{PieceID: 1, MatchingColumn: 1}},
				{Value: [][]string{{
					"15.00000000000000000000000000000000000000000000000000",
					"106.00000000000000000000000000000000000000000000000000"}},
					DataID: "7817e5c7355dd7439e28d3af0d7720f128d320f1c9fe6a3837522fff87f68411",
					Meta:   m2db.ShareMeta{PieceID: 2, MatchingColumn: 1}},
			},
		},
		"DiffPieceCase2": {
			baseData: []m2db.Share{
				{Value: [][]string{{"1", "2"}}, Meta: m2db.ShareMeta{PieceID: 0, MatchingColumn: 1}},
				{Value: [][]string{{"3", "4"}, {"5", "6"}}, Meta: m2db.ShareMeta{PieceID: 1, MatchingColumn: 1}},
			},
			addData: []m2db.Share{
				{Value: [][]string{{"10", "100"}}, Meta: m2db.ShareMeta{PieceID: 0, MatchingColumn: 1}},
				{Value: [][]string{{"10", "100"}}, Meta: m2db.ShareMeta{PieceID: 1, MatchingColumn: 1}},
				{Value: [][]string{{"10", "100"}}, Meta: m2db.ShareMeta{PieceID: 2, MatchingColumn: 1}},
			},
			expected: []m2db.Share{
				{Value: [][]string{{
					"11.00000000000000000000000000000000000000000000000000",
					"102.00000000000000000000000000000000000000000000000000"}},
					DataID: "083ee381e2733c9a6147ea050b2d6e79b494e61b34c046b4dc110b7a40d8aa91",
					Meta:   m2db.ShareMeta{PieceID: 0, MatchingColumn: 1}},
				{Value: [][]string{
					{"13.00000000000000000000000000000000000000000000000000",
						"104.00000000000000000000000000000000000000000000000000"},
					{"15.00000000000000000000000000000000000000000000000000",
						"106.00000000000000000000000000000000000000000000000000"}},
					DataID: "083ee381e2733c9a6147ea050b2d6e79b494e61b34c046b4dc110b7a40d8aa91",
					Meta:   m2db.ShareMeta{PieceID: 1, MatchingColumn: 1}},
			},
		},
	}
	for name, tt := range testcases {
		name := name
		tt := tt
		t.Run(name, func(t *testing.T) {
			t.Parallel()

			// テスト用データを1行ずつテスト用DBに保存しておく
			baseDataID := fmt.Sprintf("TestAddPieceShareDataFrameSave%s%d", name, 1)
			addDataID := fmt.Sprintf("TestAddPieceShareDataFrameSave%s%d", name, 2)
			mu.Lock()
			db[baseDataID] = map[int32]m2db.Share{}
			for pieceID, share := range tt.baseData {
				db[baseDataID][int32(pieceID)] = share
			}
			db[addDataID] = map[int32]m2db.Share{}
			for pieceID, share := range tt.addData {
				db[addDataID][int32(pieceID)] = share
			}
			mu.Unlock()

			// 加算処理をしてDBに加算された値が保存されているか確認
			p := processer{m2dbclient: localDb{}}
			dataID, errAdd := p.addShareDataFrame(baseDataID, addDataID)
			if errAdd != nil {
				t.Fatal(errAdd)
			}
			for pieceID := 0; pieceID < len(tt.baseData); pieceID++ {
				share, errShare := p.m2dbclient.GetSharePiece(dataID, int32(pieceID))
				if errShare != nil {
					t.Fatal(errShare)
				}
				if !reflect.DeepEqual(share, tt.expected[pieceID]) {
					t.Fatalf("share must be %v, but %v", tt.expected[pieceID], share)
				}
			}
		})
	}
}

// 同じIDを指定して加算したテーブルが正しく保存されてるかテスト
func TestAddShareDataFrameSaveTwice(t *testing.T) {
	testcases := map[string]struct {
		baseData m2db.Share
		addData  m2db.Share
		expected m2db.Share
	}{
		"NormalCase": {
			baseData: m2db.Share{Value: [][]string{{"1", "2"}, {"3", "4"}},
				Meta: m2db.ShareMeta{PieceID: 0, MatchingColumn: 1}},
			addData: m2db.Share{Value: [][]string{{"10", "100"}, {"10", "100"}},
				Meta: m2db.ShareMeta{PieceID: 0, MatchingColumn: 1}},
			expected: m2db.Share{Value: [][]string{
				{"11.00000000000000000000000000000000000000000000000000",
					"102.00000000000000000000000000000000000000000000000000"},
				{"13.00000000000000000000000000000000000000000000000000",
					"104.00000000000000000000000000000000000000000000000000"},
			},
				DataID: "8508368950a44ff61bcf86db613fcf9bc2c4b409d039dec6efcfc0a25765be5c",
				Meta:   m2db.ShareMeta{PieceID: 0, MatchingColumn: 1}},
		},
	}
	for name, tt := range testcases {
		name := name
		tt := tt
		t.Run(name, func(t *testing.T) {
			t.Parallel()

			// テスト用データをテスト用DBに保存しておく
			dataID1 := fmt.Sprintf("TestAddShareDataFrameSaveTwice%s%d", name, 1)
			dataID2 := fmt.Sprintf("TestAddShareDataFrameSaveTwice%s%d", name, 2)
			mu.Lock()
			db[dataID1] = map[int32]m2db.Share{}
			db[dataID1][0] = tt.baseData
			db[dataID2] = map[int32]m2db.Share{}
			db[dataID2][0] = tt.addData
			mu.Unlock()

			// 何回も加算処理をしてDBに加算された値が保存されているか確認
			for i := 0; i <= 5; i++ {
				p := processer{m2dbclient: localDb{}}
				dataID, errAdd := p.addShareDataFrame(dataID1, dataID2)
				if errAdd != nil {
					t.Fatal(errAdd)
				}
				share, errShare := p.m2dbclient.GetSharePiece(dataID, 0)
				if errShare != nil {
					t.Fatal(errShare)
				}
				if !reflect.DeepEqual(share, tt.expected) {
					t.Fatalf("share must be %v, but %v", tt.expected, share)
				}
			}
		})
	}
}
