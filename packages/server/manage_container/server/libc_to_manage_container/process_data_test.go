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
				DataID: "6a8787d0da13f1dc67fce89cbb217aeeb5473fb1dce55e846ab65f35135dd215",
				Meta:   m2db.ShareMeta{PieceID: 0, MatchingColumn: 1}},
		},
	}
	for name, tt := range testcases {
		name := name
		tt := tt
		t.Run(name, func(t *testing.T) {
			t.Parallel()

			// テスト用データをテスト用DBに保存しておく
			dataID1 := fmt.Sprintf("%s%d", name, 1)
			dataID2 := fmt.Sprintf("%s%d", name, 2)
			db[dataID1] = map[int32]m2db.Share{}
			db[dataID1][0] = tt.baseData
			db[dataID2] = map[int32]m2db.Share{}
			db[dataID2][0] = tt.addData

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
					DataID: "6a8787d0da13f1dc67fce89cbb217aeeb5473fb1dce55e846ab65f35135dd215",
					Meta:   m2db.ShareMeta{PieceID: 0, MatchingColumn: 1}},
				{Value: [][]string{{
					"13.00000000000000000000000000000000000000000000000000",
					"104.00000000000000000000000000000000000000000000000000"}},
					DataID: "6a8787d0da13f1dc67fce89cbb217aeeb5473fb1dce55e846ab65f35135dd215",
					Meta:   m2db.ShareMeta{PieceID: 1, MatchingColumn: 1}},
				{Value: [][]string{{
					"15.00000000000000000000000000000000000000000000000000",
					"106.00000000000000000000000000000000000000000000000000"}},
					DataID: "6a8787d0da13f1dc67fce89cbb217aeeb5473fb1dce55e846ab65f35135dd215",
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
					DataID: "bbcc21770ff7bf4b711243017e94d8493437d92a8152aa7da7b0be276096fc80",
					Meta:   m2db.ShareMeta{PieceID: 0, MatchingColumn: 1}},
				{Value: [][]string{{
					"13.00000000000000000000000000000000000000000000000000",
					"104.00000000000000000000000000000000000000000000000000"}},
					DataID: "bbcc21770ff7bf4b711243017e94d8493437d92a8152aa7da7b0be276096fc80",
					Meta:   m2db.ShareMeta{PieceID: 1, MatchingColumn: 1}},
				{Value: [][]string{{
					"15.00000000000000000000000000000000000000000000000000",
					"106.00000000000000000000000000000000000000000000000000"}},
					DataID: "bbcc21770ff7bf4b711243017e94d8493437d92a8152aa7da7b0be276096fc80",
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
					DataID: "184642e3b73a954582fb59d6feb3f0a95a458923bc3ef033059f4b64e6032cc1",
					Meta:   m2db.ShareMeta{PieceID: 0, MatchingColumn: 1}},
				{Value: [][]string{
					{"13.00000000000000000000000000000000000000000000000000",
						"104.00000000000000000000000000000000000000000000000000"},
					{"15.00000000000000000000000000000000000000000000000000",
						"106.00000000000000000000000000000000000000000000000000"}},
					DataID: "184642e3b73a954582fb59d6feb3f0a95a458923bc3ef033059f4b64e6032cc1",
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
			baseDataID := fmt.Sprintf("%s%d", name, 1)
			addDataID := fmt.Sprintf("%s%d", name, 2)
			db[baseDataID] = map[int32]m2db.Share{}
			for pieceID, share := range tt.baseData {
				db[baseDataID][int32(pieceID)] = share
			}
			db[addDataID] = map[int32]m2db.Share{}
			for pieceID, share := range tt.addData {
				db[addDataID][int32(pieceID)] = share
			}

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
