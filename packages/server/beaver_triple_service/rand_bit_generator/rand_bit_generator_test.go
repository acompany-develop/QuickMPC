package randbitgenerator_test

import (
	"os"
	"fmt"
	"testing"
	"sync"

	jwt_types "github.com/acompany-develop/QuickMPC/packages/server/beaver_triple_service/jwt"
	utils "github.com/acompany-develop/QuickMPC/packages/server/beaver_triple_service/utils"
	rbg "github.com/acompany-develop/QuickMPC/packages/server/beaver_triple_service/rand_bit_generator"
	rbs "github.com/acompany-develop/QuickMPC/packages/server/beaver_triple_service/rand_bit_store"
	pb "github.com/acompany-develop/QuickMPC/proto/engine_to_bts"
)

type RandBitsStock struct{
	Stock map[uint32](map[uint32]([]*int64))
	Mux     sync.Mutex
}
var RBS RandBitsStock

func all_init() {
	RBS.Stock = make(map[uint32](map[uint32]([]*int64)))
	rbg.Db = &rbs.SafeRandBitStore{
		RandBits: make(map[uint32](map[uint32]([]*int64))),
		PreID: make(map[uint32](map[uint32](int64))),
		PreAmount: make(map[uint32](map[uint32](uint32))),
	}
}

func getClaims() (*jwt_types.Claim, error){
	token, ok := os.LookupEnv("BTS_TOKEN")
	if ok {
		claims,err := utils.AuthJWT(token)
		if err != nil {
			return nil,err
		}
		return claims,nil
	}

	return nil,fmt.Errorf("BTS TOKEN is not valified")
}

// 固定された (jobId, partyId) に対し requestTimes 回の（長さ amount の） RandBits を作成
func multiGetRandBits(t *testing.T, jobId uint32, partyId uint32, amount uint32, bit_type pb.Type, requestTimes uint32) {
	t.Helper()

	claims, err := getClaims()
	if err != nil {
		t.Fatal(err)
	}

	RBS.Mux.Lock()

	_, ok := RBS.Stock[jobId]
	if !ok{
		RBS.Stock[jobId] = make(map[uint32]([]*int64))
	}

	RBS.Mux.Unlock()

	t.Run(fmt.Sprintf("TestRandBitGenerator_Job%d", jobId), func(t *testing.T) {
		for loopRequestId := uint32(0); loopRequestId < requestTimes; loopRequestId++ {
			requestId := int64(loopRequestId)
			bits, err := rbg.GetRandBits(claims, jobId, partyId, amount, value_type, requestId)
			if err != nil {
				t.Fatal(err)
			}

			RBS.Mux.Lock()

			_, ok := RBS.Stock[jobId][partyId]
			if ok {
				RBS.Stock[jobId][partyId] = append(RBS.Stock[jobId][partyId], bits..)
			} else {
				RBS.Stock[jobId][partyId] = bits
			}

			RBS.Mux.Unlock()
		}
	})
}

func testValidityOfRandBits(t *testing.T) {
	for _, PartyToRandBits := range RBS.Stock {
		for i := 0; i < len(PartyToRandBits[1]); i++ {
			bitSum := int64(0)
			for partyId := uint32(1); partyId <= uint32(len(PartyToRandBits)); partyId++ {
				bitSum += PartyToRandBits[partyId]
			}
			if bitSum != 0 && bitSum != 1{
				t.Fatal("This is not bit")
			}
		}
	}
}

func parallelGetRandBits(t *testing.T, jobNum uint32, amount uint32, value_type pb.Type, requestTime uint32) {
	t.Helper()

	all_init()

	claims, err := getClaims()
	if err != nil {
		t.Fatal(err)
	}

	for loopJobId := uint32(0); loopJobId < jobNum; loopJobId++ {
		for loopPartyId := uint32(1); loopPartyId <= uint32(len(claims.PartyInfo)); loopPartyId++ {
			jobId := loopJobId
			partyId := loopPartyId
			t.Run("TestRandBitGenerator", func(t *testing.T){
				t.Parallel()
				multiGetRandBits(t, jobId, partyId, amount, value_type, requestTime)
			})
		}
	}

	t.Cleanup(func(){
		testValidityOfRandBits(t)
	})
}

func testParallelGetRandBits_FP(t *testing.T, jobNum uint32, amount uint32, requestTime uint32){
	parallelGetRandBits(t, jobNum, amount, pb.Type_TYPE_FIXEDPOINT, requestTime)
}

func testParallelGetRandBits_Float(t *testing.T, jobNum uint32, amount uint32, requestTime uint32){
	parallelGetRandBits(t, jobNum, amount, pb.Type_TYPE_FLOAT, requestTime)
}

// --- 以下呼ばれる関数群 ---
func TestParallelGetRandBits_FP_1_1_1(t *testing.T){
	testParallelGetRandBits_FP(t, 1, 1, 1)
}
func TestParallelGetRandBits_FP_10_10_10(t *testing.T){
	testParallelGetRandBits_FP(t, 10, 10, 10)
}
func TestParallelGetRandBits_FP_10000_5_5(t *testing.T){
	testParallelGetRandBits_FP(t, 10000, 5, 5)
}
func TestParallelGetRandBits_FP_5_10000_5(t *testing.T){
	testParallelGetRandBits_FP(t, 5, 10000, 5)
}
func TestParallelGetRandBits_FP_5_5_10000(t *testing.T){
	testParallelGetRandBits_FP(t, 5, 5, 10000)
}

func TestParallelGetRandBits_Float_1_1_1(t *testing.T){
	testParallelGetRandBits_Float(t, 1, 1, 1)
}
func TestParallelGetRandBits_Float_10_10_10(t *testing.T){
	testParallelGetRandBits_Float(t, 10, 10, 10)
}
func TestParallelGetRandBits_Float_10000_5_5(t *testing.T){
	testParallelGetRandBits_Float(t, 10000, 5, 5)
}
func TestParallelGetRandBits_Float_5_10000_5(t *testing.T){
	testParallelGetRandBits_Float(t, 5, 10000, 5)
}
func TestParallelGetRandBits_Float_5_5_10000(t *testing.T){
	testParallelGetRandBits_Float(t, 5, 5, 10000)
}

// 同じ request ID は同じ RandBit
func TestSameRequestId(t *testing.T){
	t.Helper()

	all_init()

	claims, err := getClaims()
	if err != nil {
		t.Fatal(err)
	}

	// jobId が他の Test と被らないようにする
	jobId := uint32(12345678)
	amount := uint32(1000)
	value_type := pb.Type_TYPE_FIXEDPOINT
	for partyId := uint32(1); partyId <= uint32(len(claims.PartyInfo)); partyId++ {
		randbits1, err := rbg.GetRandBits(claims, jobId, partyId, amount, value_type, 1)
		if err != nil {
			t.Fatal(err)
		}
		randbits2, err := rbg.GetRandBits(claims, jobId, partyId, amount, value_type, 1)
		if err != nil {
			t.Fatal(err)
		}
		for i := uint32(0); i < amount; i++ {
			if randbits1[i] != randbits2[i] {
				t.Fatal("same requestId different randbits")
			}
		}
	}
}

// 異なる request ID は異なる RandBit
func TestDifferentRequestId(t *testing.T){
	t.Helper()

	all_init()

	claims, err := getClaims()
	if err != nil {
		t.Fatal(err)
	}

	// jobId が他の Test と被らないようにする
	jobId := uint32(87654321)
	amount := uint32(1000)
	value_type := pb.Type_TYPE_FIXEDPOINT
	for partyId := uint32(1); partyId <= uint32(len(claims.PartyInfo)); partyId++ {
		randbits1, err := rbg.GetRandBits(claims, jobId, partyId, amount, value_type, 1)
		if err != nil {
			t.Fatal(err)
		}
		randbits2, err := rbg.GetRandBits(claims, jobId, partyId, amount, value_type, 2)
		if err != nil {
			t.Fatal(err)
		}
		for i := uint32(0); i < amount; i++ {
			if randbits1[i] == randbits2[i] {
				t.Fatal("different requestId same randbits")
			}
		}
	}
}

// 範囲外の PartyId が来た時にエラーを吐くか
func TestOutRangePartyId(t *testing.T){
	expected_text := "out range partyId"
	value_type := pb.Type_TYPE_FIXEDPOINT
	claims, err := getClaims()
	if err != nil {
		t.Fatal(err)
	}

	partyId := uint32(0)
	if _, err := rbg.GetRandBits(claims, 1, partyId, 1, value_type, -1); err.Error() != expected_text{
		t.Fatal("does not output 'out range partyId'")
	}

	partyId = uint32(len(claims.PartyInfo))  + uint32(1)
	if _, err := rbg.GetRandBits(claims, 1, partyId, 1, value_type, -1); err.Error() != expected_text{
		t.Fatal("does not output 'out range partyId'")
	}
}