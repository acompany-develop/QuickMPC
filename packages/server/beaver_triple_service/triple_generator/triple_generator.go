package triplegenerator

import (
	"errors"

	jwt_types "github.com/acompany-develop/QuickMPC/packages/server/beaver_triple_service/jwt"
	logger "github.com/acompany-develop/QuickMPC/packages/server/beaver_triple_service/log"
	ts "github.com/acompany-develop/QuickMPC/packages/server/beaver_triple_service/triple_store"
	utils "github.com/acompany-develop/QuickMPC/packages/server/beaver_triple_service/utils"
	pb "github.com/acompany-develop/QuickMPC/proto/engine_to_bts"
)

var Db *ts.SafeTripleStore
var tripleRandMax = int64(1000)
var tripleRandMin = int64(-1000)

// floatの場合の乱数範囲はエンジン側のgetRandShareに依存している
// 他の用途で使用する場合は範囲を再検討，もしくは分岐を再設計する
var sharizeRandMinMap = map[pb.Type]int64{
	pb.Type_TYPE_FLOAT:      -1000,
	pb.Type_TYPE_FIXEDPOINT: int64(-1 << 60),
}
var sharizeRandMaxMap = map[pb.Type]int64{
	pb.Type_TYPE_FLOAT:      1000,
	pb.Type_TYPE_FIXEDPOINT: int64(1 << 60),
}

func init() {
	Db = ts.GetInstance()
}

func sharize(data int64, size uint32, triple_type pb.Type) ([]int64, error) {
	if triple_type == pb.Type_TYPE_UNKNOWN {
		errText := "RequestにTripleの型情報が含まれていません．"
		logger.Error(errText)
		return nil, errors.New(errText)
	}
	sharizeRandMin := sharizeRandMinMap[triple_type]
	sharizeRandMax := sharizeRandMaxMap[triple_type]
	shares, err := utils.GetRandInt64Slice(uint64(size-1), sharizeRandMin, sharizeRandMax)
	if err != nil {
		errText := "乱数取得に失敗"
		logger.Error(errText)
		return nil, errors.New(errText)
	}

	sum := int64(0)
	for _, x := range shares {
		sum += x
	}

	shares = append(shares, data-sum)
	return shares, nil
}

func GenerateTriples(claims *jwt_types.Claim, amount uint32, triple_type pb.Type) (map[uint32]([]*ts.Triple), error) {
	ret := make(map[uint32]([]*ts.Triple))

	for i := uint32(0); i < amount; i++ {
		randInt64Slice, err := utils.GetRandInt64Slice(2, tripleRandMin, tripleRandMax)
		if err != nil {
			errText := "乱数取得に失敗"
			logger.Error(errText)
			return nil, errors.New(errText)
		}

		a := randInt64Slice[0]
		b := randInt64Slice[1]
		c := a * b

		party_num := uint32(len(claims.PartyInfo))
		aShares, err := sharize(a, party_num, triple_type)
		if err != nil {
			return nil, err
		}
		bShares, err := sharize(b, party_num, triple_type)
		if err != nil {
			return nil, err
		}
		cShares, err := sharize(c, party_num, triple_type)
		if err != nil {
			return nil, err
		}

		// partyIdは1-index
		for partyId := uint32(1); partyId <= party_num; partyId++ {
			t := ts.Triple{
				A: aShares[partyId-1],
				B: bShares[partyId-1],
				C: cShares[partyId-1],
			}
			ret[partyId] = append(ret[partyId], &t)
		}
	}

	return ret, nil
}

func GetTriples(claims *jwt_types.Claim, jobId uint32, partyId uint32, amount uint32, triple_type pb.Type, requestId int64) ([]*ts.Triple, error) {
	Db.Mux.Lock()
	defer Db.Mux.Unlock()

	// jobId が初めての場合
	if _, ok := Db.PreID[jobId]; !ok {
		Db.PreID[jobId] = make(map[uint32](int64))
		Db.PreAmount[jobId] = make(map[uint32](uint32))
		Db.Triples[jobId] = make(map[uint32]([]*pb.Triple))
	}

	pre_id, ok := Db.PreID[jobId][partyId]

	// request が初めての場合
	if !ok{
		Db.PreID[jobId][partyId] = requestId
		Db.PreAmount[jobId][partyId] = amount
	}

	// 前回の request と異なる場合
	if ok && pre_id != requestId && requestId != -1{
		pre_amount := Db.PreAmount[jobId][partyId]
		Db.Triples[jobId][partyId] = Db.Triples[jobId][partyId][pre_amount:]
		Db.PreID[jobId][partyId] = requestId
		Db.PreAmount[jobId][partyId] = amount
	}

	// 今回返す Triples がまだ生成されてない場合
	if len(Db.Triples[jobId][partyId]) == 0{
		newTriples, err := GenerateTriples(claims, amount, triple_type)
		if err != nil {
			return nil, err
		}
		// partyIdは1-index
		for partyId := uint32(1); partyId <= uint32(len(claims.PartyInfo)); partyId++ {
			_, ok := Db.Triples[jobId][partyId]
			if ok {
				Db.Triples[jobId][partyId] = append(Db.Triples[jobId][partyId], newTriples[partyId]...)
			} else {
				Db.Triples[jobId][partyId] = newTriples[partyId]
			}
		}
	}

	triples := Db.Triples[jobId][partyId][:amount]

	return triples, nil
}

func DeleteJobIdTriple(jobId uint32) error {
	// jobIdに紐付いたTripleを削除
	// delete(Db.Triples, jobId)
	return nil
}
