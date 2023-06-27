package l2mserver

import (
	"crypto/sha256"
	"encoding/hex"
	"fmt"
	"math/big"

	m2db "github.com/acompany-develop/QuickMPC/packages/server/manage_container/client/manage_to_db"
	utils "github.com/acompany-develop/QuickMPC/packages/server/manage_container/utils"
)

type processer struct {
	m2dbclient m2db.M2DbClient
}

func (p processer) addShareDataFrame(baseDataID string, addDataID string) (string, error) {
	// pieceサイズの取得(とデータの存在確認)
	basePieceSize, errBasePiece := p.m2dbclient.GetSharePieceSize(baseDataID)
	if errBasePiece != nil {
		return "", errBasePiece
	}
	addPieceSize, errAddPiece := p.m2dbclient.GetSharePieceSize(addDataID)
	if errAddPiece != nil {
		return "", errAddPiece
	}

	// 新しいデータIDの生成
	s := fmt.Sprintf("%s+%s", baseDataID, addDataID)
	r := sha256.Sum256([]byte(s))
	dataID := hex.EncodeToString(r[:])

	// pieceのindexと今見ているpieceデータのindex
	basePieceID := int32(0)
	addPieceID := int32(0)
	baseItr := 0
	addItr := 0
	baseShare, _ := p.m2dbclient.GetSharePiece(baseDataID, int32(basePieceID))
	addShare, _ := p.m2dbclient.GetSharePiece(addDataID, int32(addPieceID))
	for basePieceID < basePieceSize || addPieceID < addPieceSize {
		// baseShareの今見ているpieceを読み終わったら一旦保存して次のpieceへ
		if basePieceID < basePieceSize && len(baseShare.Value) == baseItr {
			shareJSONStr, errConvert := utils.ConvertToJsonstr(baseShare.Value)
			if errConvert != nil {
				p.m2dbclient.DeleteShares([]string{dataID})
				return "", errConvert
			}
			errInsert := p.m2dbclient.InsertShares(dataID, baseShare.Meta.Schema, int32(basePieceID), shareJSONStr, baseShare.SentAt, baseShare.Meta.MatchingColumn)
			if errInsert != nil {
				p.m2dbclient.DeleteShares([]string{dataID})
				return "", errInsert
			}

			basePieceID++
			baseItr = 0
			baseShare, _ = p.m2dbclient.GetSharePiece(baseDataID, int32(basePieceID))
			continue
		}
		// addShareの今見ているpieceを読み終わったら次のpieceへ
		if addPieceID < addPieceSize && len(addShare.Value) == addItr {
			addPieceID++
			addItr = 0
			addShare, _ = p.m2dbclient.GetSharePiece(addDataID, int32(addPieceID))
			continue
		}
		// 両方のpieceを全て見終わったら終了
		if basePieceID == basePieceSize && addPieceID == addPieceSize {
			break
		}
		// 片方のpieceを見終わったのにもう片方が残ってたらサイズが違うのでerror
		if basePieceID == basePieceSize || addPieceID == addPieceSize {
			p.m2dbclient.DeleteShares([]string{dataID})
			return "", fmt.Errorf("ERROR! AddShare row Size is not equal baseShare row Size")
		}
		// column数が違ったらerror
		if len(baseShare.Value[baseItr]) != len(addShare.Value[addItr]) {
			p.m2dbclient.DeleteShares([]string{dataID})
			return "", fmt.Errorf("ERROR! AddShare column size is not equal baseShare column Size")
		}

		// 加算処理，baseShareに対して破壊的に加算する
		// TODO: 桁数を適切な値に変更する(そもそも整数にして良くなりそう)
		for i := 0; i < len(baseShare.Value[baseItr]); i++ {
			a, _ := new(big.Rat).SetString(baseShare.Value[baseItr][i])
			b, _ := new(big.Rat).SetString(addShare.Value[addItr][i])
			sum := new(big.Rat).Add(a, b).FloatString(50)
			baseShare.Value[baseItr][i] = sum
		}

		addItr++
		baseItr++
	}
	return dataID, nil
}
