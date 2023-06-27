package l2mserver

import (
	"fmt"
	"math/big"

	m2db "github.com/acompany-develop/QuickMPC/packages/server/manage_container/client/manage_to_db"
	utils "github.com/acompany-develop/QuickMPC/packages/server/manage_container/utils"
)

type processer struct {
	m2dbclient m2db.M2DbClient
}

func (p processer) addShareDataFrame(baseDataID string, addDataID string) (string, error) {
	basePieceSize, errBasePiece := p.m2dbclient.GetSharePieceSize(baseDataID)
	if errBasePiece != nil {
		return "", errBasePiece
	}
	addPieceSize, errAddPiece := p.m2dbclient.GetSharePieceSize(addDataID)
	if errAddPiece != nil {
		return "", errAddPiece
	}
	dataID := "str"

	basePieceID := int32(0)
	addPieceID := int32(0)
	baseItr := 0
	addItr := 0
	baseShare, _ := p.m2dbclient.GetSharePiece(baseDataID, int32(basePieceID))
	addShare, _ := p.m2dbclient.GetSharePiece(addDataID, int32(addPieceID))
	for basePieceID < basePieceSize || addPieceID < addPieceSize {
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
		if addPieceID < addPieceSize && len(addShare.Value) == addItr {
			addPieceID++
			addItr = 0
			addShare, _ = p.m2dbclient.GetSharePiece(addDataID, int32(addPieceID))
			continue
		}
		if basePieceID == basePieceSize && addPieceID == addPieceSize {
			break
		}
		if basePieceID == basePieceSize || addPieceID == addPieceSize {
			p.m2dbclient.DeleteShares([]string{dataID})
			return "", fmt.Errorf("ERROR! AddShare row Size is not equal baseShare row Size")
		}
		if len(baseShare.Value[baseItr]) != len(addShare.Value[addItr]) {
			p.m2dbclient.DeleteShares([]string{dataID})
			return "", fmt.Errorf("ERROR! AddShare column size is not equal baseShare column Size")
		}

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
