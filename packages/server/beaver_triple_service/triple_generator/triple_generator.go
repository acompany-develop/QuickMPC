package triplegenerator

import (
	"errors"

	cs "github.com/acompany-develop/QuickMPC/packages/server/beaver_triple_service/config_store"
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

func GenerateTriples(amount uint32, triple_type pb.Type) (map[uint32]([]*ts.Triple), error) {
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

		aShares, err := sharize(a, cs.Conf.PartyNum, triple_type)
		if err != nil {
			return nil, err
		}
		bShares, err := sharize(b, cs.Conf.PartyNum, triple_type)
		if err != nil {
			return nil, err
		}
		cShares, err := sharize(c, cs.Conf.PartyNum, triple_type)
		if err != nil {
			return nil, err
		}

		// partyIdは1-index
		for partyId := uint32(1); partyId <= cs.Conf.PartyNum; partyId++ {
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

func GetTriples(jobId uint32, partyId uint32, amount uint32, triple_type pb.Type) ([]*ts.Triple, error) {
	Db.Mux.Lock()
	defer Db.Mux.Unlock()

	if len(Db.Triples[jobId]) == 0 {
		newTriples, err := GenerateTriples(amount, triple_type)
		if err != nil {
			return nil, err
		}

		Db.Triples[jobId] = newTriples
	}

	var triples []*ts.Triple
	_, ok := Db.Triples[jobId][partyId]

	// とあるパーティの複数回目のリクエストが、他パーティより先行されても対応できるように全パーティに triple をappendする
	if !ok {
		newTriples, err := GenerateTriples(amount, triple_type)
		if err != nil {
			return nil, err
		}

		// partyIdは1-index
		for partyId := uint32(1); partyId <= cs.Conf.PartyNum; partyId++ {
			_, ok := Db.Triples[jobId][partyId]
			if ok {
				Db.Triples[jobId][partyId] = append(Db.Triples[jobId][partyId], newTriples[partyId]...)
			} else {
				Db.Triples[jobId][partyId] = newTriples[partyId]
			}
		}
	}

	triples = Db.Triples[jobId][partyId][:amount]
	Db.Triples[jobId][partyId] = Db.Triples[jobId][partyId][amount:]
	if len(Db.Triples[jobId][partyId]) == 0 {
		delete(Db.Triples[jobId], partyId)
	}

	if len(triples) == 0 {
		errText := "すでに取得済みのリソースがリクエストされた"
		logger.Error(errText)
		return nil, errors.New(errText)
	}

	// 全て配り終わったら削除
	if len(Db.Triples[jobId]) == 0 {
		delete(Db.Triples, jobId)
	}

	return triples, nil
}

func InitTripleStore() error {
	// Dbを初期化
	Db.Triples = map[uint32](map[uint32]([]*pb.Triple)){}
	return nil
}

func DeleteJobIdTriple(jobId uint32) error {
	// jobIdに紐付いたTripleを削除
	delete(Db.Triples, jobId)
	return nil
}
