package triplegenerator

import (
	"errors"
	"math/big"

	jwt_types "github.com/acompany-develop/QuickMPC/packages/server/beaver_triple_service/jwt"
	logger "github.com/acompany-develop/QuickMPC/packages/server/beaver_triple_service/log"
	ts "github.com/acompany-develop/QuickMPC/packages/server/beaver_triple_service/triple_store"
	utils "github.com/acompany-develop/QuickMPC/packages/server/beaver_triple_service/utils"
	pb "github.com/acompany-develop/QuickMPC/proto/engine_to_bts"
)

var Db *ts.SafeTripleStore

// a,b の生成範囲は 10^{19+8} ~ 2^{90}
var tripleBitLength uint32 = 90

func init() {
	Db = ts.GetInstance()
}

func sharize(data big.Int, bitLength uint32, party_num uint32) ([]big.Int, error) {
	shares, err := utils.GetRandBigInts(bitLength, party_num - 1)
	if err != nil {
		errText := "乱数取得に失敗"
		logger.Error(errText)
		return nil, errors.New(errText)
	}

	for _, share := range shares {
		data.Sub(&data, &share)
	}
	shares = append(shares, data)

	return shares, nil
}

func toBytes(share big.Int) (pb.BigIntByte, error) {
	ret := pb.BigIntByte{
		Sgn : share.Sign() == -1,
		Byte : share.Bytes(),
	}
	return ret, nil
}

func GenerateTriples(claims *jwt_types.Claim, amount uint32) (map[uint32]([]*ts.Triple), error) {
	ret := make(map[uint32]([]*ts.Triple))
	party_num := uint32(len(claims.PartyInfo))

	for partyId := uint32(1); partyId <= party_num; partyId++ {
		ret[partyId] = []*ts.Triple{}
	}

	ab, err := utils.GetRandBigInts(tripleBitLength, 2*amount)
	if err != nil {
		errText := "乱数取得に失敗"
		logger.Error(errText)
		return nil, errors.New(errText)
	}

	for i := uint32(0); i < 2*amount; i+=2 {
		var a big.Int = ab[i]
		var b big.Int = ab[i]
		var c big.Int
		c.Mul(&a, &b)

		aShares, err := sharize(a, tripleBitLength, party_num)
		if err != nil {
			return nil, err
		}
		bShares, err := sharize(b, tripleBitLength, party_num)
		if err != nil {
			return nil, err
		}
		cShares, err := sharize(c, 2*tripleBitLength, party_num)
		if err != nil {
			return nil, err
		}

		// partyIdは1-index
		for partyId := uint32(1); partyId <= party_num; partyId++ {
			a_, err := toBytes(aShares[partyId-1])
			if err != nil{
				return nil, err
			}
			b_, err := toBytes(bShares[partyId-1])
			if err != nil{
				return nil, err
			}
			c_, err := toBytes(cShares[partyId-1])
			if err != nil{
				return nil, err
			}
			t := ts.Triple{
				A: &a_,
				B: &b_,
				C: &c_,
			}
			ret[partyId] = append(ret[partyId], &t)
		}
	}

	return ret, nil
}

func GetTriples(claims *jwt_types.Claim, jobId uint32, partyId uint32, amount uint32, requestId int64) ([]*ts.Triple, error) {
	Db.Mux.Lock()
	defer Db.Mux.Unlock()

	if partyId == 0 || partyId > uint32(len(claims.PartyInfo)){
		errText := "out range partyId"
		logger.Error(errText)
		return nil, errors.New(errText)
	}

	// jobId が初めての場合
	if _, ok := Db.PreID[jobId]; !ok {
		Db.Triples[jobId] = make(map[uint32]([]*pb.Triple))
		Db.PreID[jobId] = make(map[uint32](int64))
		Db.PreAmount[jobId] = make(map[uint32](uint32))
	}

	pre_id, ok := Db.PreID[jobId][partyId]

	// request が初めての場合
	if !ok{
		Db.PreID[jobId][partyId] = requestId
		Db.PreAmount[jobId][partyId] = amount
	}

	// 前回の request と異なる場合
	// requestId が -1 の場合は必ず前回と異なるとみなす（test用）
	if ok && (pre_id != requestId || requestId == -1){
		pre_amount := Db.PreAmount[jobId][partyId]
		Db.Triples[jobId][partyId] = Db.Triples[jobId][partyId][pre_amount:]
		Db.PreID[jobId][partyId] = requestId
		Db.PreAmount[jobId][partyId] = amount
	}

	// 今回返す Triples がまだ生成されてない場合
	if len(Db.Triples[jobId][partyId]) == 0{
		newTriples, err := GenerateTriples(claims, amount)
		if err != nil {
			return nil, err
		}
		for loopPartyId := uint32(1); loopPartyId <= uint32(len(claims.PartyInfo)); loopPartyId++ {
			_, ok := Db.Triples[jobId][loopPartyId]
			if ok {
				Db.Triples[jobId][loopPartyId] = append(Db.Triples[jobId][loopPartyId], newTriples[loopPartyId]...)
			} else {
				Db.Triples[jobId][loopPartyId] = newTriples[loopPartyId]
			}
		}
	}

	triples := Db.Triples[jobId][partyId][:amount]

	return triples, nil
}