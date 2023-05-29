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

func (p processer) addValueToIDCol(dataID string, value []string) error {
	pieceSize, errPiece := p.m2dbclient.GetSharePieceSize(dataID)
	if errPiece != nil {
		return errPiece
	}
	sampleShare, errSample := p.m2dbclient.GetSharePiece(dataID, 0)
	if errSample != nil {
		return errSample
	}

	// NOTE: rollbackのためにerr判定だけ先に行う
	shareSize := 0
	for pieceID := 0; pieceID < int(pieceSize); pieceID++ {
		share, err := p.m2dbclient.GetSharePiece(dataID, int32(pieceID))
		if err != nil {
			return err
		}
		shareSize += len(share.Value)
	}
	if len(value) != shareSize {
		return fmt.Errorf("ERROR! AddValueSize not equal shareSize. addValueSize=%d, shareSize=%d", len(value), shareSize)
	}

	vi := 0
	matchingColumn := sampleShare.Meta.MatchingColumn
	for pieceID := 0; pieceID < int(pieceSize); pieceID++ {
		share, _ := p.m2dbclient.GetSharePiece(dataID, int32(pieceID))
		for i, row := range share.Value {
			a, _ := new(big.Rat).SetString(row[matchingColumn-1])
			b, _ := new(big.Rat).SetString(value[vi])
			sum := new(big.Rat).Add(a, b).FloatString(50)
			share.Value[i][matchingColumn-1] = sum
			vi++
		}

		shareJsonStr, err := utils.ConvertToJsonstr(share)
		if err != nil {
			return err
		}
		p.m2dbclient.DeleteShares([]string{dataID})
		p.m2dbclient.InsertShares(dataID, sampleShare.Meta.Schema, int32(pieceID), shareJsonStr, sampleShare.SentAt, matchingColumn)
	}
	return nil
}
