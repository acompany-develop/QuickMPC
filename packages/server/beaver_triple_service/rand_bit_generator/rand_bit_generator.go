package randbitgenerator

import (
	"errors"

	jwt_types "github.com/acompany-develop/QuickMPC/packages/server/beaver_triple_service/jwt"
	logger "github.com/acompany-develop/QuickMPC/packages/server/beaver_triple_service/log"
	rbs "github.com/acompany-develop/QuickMPC/packages/server/beaver_triple_service/rand_bit_store"
	utils "github.com/acompany-develop/QuickMPC/packages/server/beaver_triple_service/utils"
)

var Db *rbs.SafeRandBitStore

func init() {
	Db = rbs.GetInstance()
}

func sharize(data int64, size uint32) ([]int64, error) {
	sharizeRandMin := - int64(1 << 60)
	sharizeRandMax := int64(1 << 60)
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

func GenerateRandBits(claims *jwt_types.Claim, amount uint32) (map[uint32]([]int64), error) {
	ret := make(map[uint32]([]int64))
	party_num := uint32(len(claims.PartyInfo))

	for partyId := uint32(1); partyId <= party_num; partyId++ {
		ret[partyId] = []int64{}
	}

	randBits, err := utils.GetRandInt64Slice(uint64(amount), 0, 1)
	if err != nil{
		return nil, err
	}

	for i := uint32(0); i < amount; i++ {
		b := randBits[i]
		shares, err := sharize(b, party_num)
		if err != nil{
			return nil, err
		}

		// partyIdは1-index
		for partyId := uint32(1); partyId <= party_num; partyId++ {
			ret[partyId] = append(ret[partyId], shares[partyId-1])
		}
	}
	return ret, nil
}

func GetRandBits(claims *jwt_types.Claim, jobId uint32, partyId uint32, amount uint32, requestId int64) ([]int64, error) {
	Db.Mux.Lock()
	defer Db.Mux.Unlock()

	if partyId == 0 || partyId > uint32(len(claims.PartyInfo)){
		errText := "out range partyId"
		logger.Error(errText)
		return nil, errors.New(errText)
	}

	// jobId が初めての場合
	if _, ok := Db.PreID[jobId]; !ok {
		Db.RandBits[jobId] = make(map[uint32]([]int64))
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
		Db.RandBits[jobId][partyId] = Db.RandBits[jobId][partyId][pre_amount:]
		Db.PreID[jobId][partyId] = requestId
		Db.PreAmount[jobId][partyId] = amount
	}

	// 今回返す Bits がまだ生成されてない場合
	if len(Db.RandBits[jobId][partyId]) == 0{
		newBits, err := GenerateRandBits(claims, amount)
		if err != nil {
			return nil, err
		}
		for loopPartyId := uint32(1); loopPartyId <= uint32(len(claims.PartyInfo)); loopPartyId++ {
			_, ok := Db.RandBits[jobId][loopPartyId]
			if ok {
				Db.RandBits[jobId][loopPartyId] = append(Db.RandBits[jobId][loopPartyId], newBits[loopPartyId]...)
			} else {
				Db.RandBits[jobId][loopPartyId] = newBits[loopPartyId]
			}
		}
	}

	randbits := Db.RandBits[jobId][partyId][:amount]

	return randbits, nil
}