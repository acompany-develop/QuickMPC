package triplegenerator_test

import (
	"os"
	"fmt"
	"testing"
	"sync"
	"math/big"

	jwt_types "github.com/acompany-develop/QuickMPC/packages/server/beaver_triple_service/jwt"
	utils "github.com/acompany-develop/QuickMPC/packages/server/beaver_triple_service/utils"
	tg "github.com/acompany-develop/QuickMPC/packages/server/beaver_triple_service/triple_generator"
	ts "github.com/acompany-develop/QuickMPC/packages/server/beaver_triple_service/triple_store"
	pb "github.com/acompany-develop/QuickMPC/proto/engine_to_bts"
)

type TriplesStock struct{
	Stock map[uint32](map[uint32]([]*ts.Triple))
	Mux     sync.Mutex
}
var TS TriplesStock

func all_init() {
	TS.Stock = make(map[uint32](map[uint32]([]*ts.Triple)))
	tg.Db = &ts.SafeTripleStore{
		Triples: make(map[uint32](map[uint32]([]*ts.Triple))),
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

// 固定された (jobId, partyId) に対し requestTimes 回の（長さ amount の） Triples を作成
func multiGetTriples(t *testing.T, jobId uint32, partyId uint32, amount uint32, requestTimes uint32) {
	t.Helper()

	claims, err := getClaims()
	if err != nil {
		t.Fatal(err)
	}

	TS.Mux.Lock()

	_, ok := TS.Stock[jobId]
	if !ok{
		TS.Stock[jobId] = make(map[uint32]([]*ts.Triple))
	}

	TS.Mux.Unlock()

	t.Run(fmt.Sprintf("TestTripleGenerator_Job%d", jobId), func(t *testing.T) {
		for loopRequestId := uint32(0); loopRequestId < requestTimes; loopRequestId++ {
			requestId := int64(loopRequestId)
			triples, err := tg.GetTriples(claims, jobId, partyId, amount, requestId)
			if err != nil {
				t.Fatal(err)
			}
			tmp := convertToBigInt(triples[0].A)

			TS.Mux.Lock()

			_, ok := TS.Stock[jobId][partyId]
			if ok {
				TS.Stock[jobId][partyId] = append(TS.Stock[jobId][partyId], triples...)
			} else {
				TS.Stock[jobId][partyId] = triples
			}

			TS.Mux.Unlock()
		}
	})
}

func convertToBigInt(b *pb.BigIntByte) (*big.Int) {
	var ret *big.Int = big.NewInt(0)
	bytes := b.AbsByte
	ret.SetBytes(bytes)
	if b.Sgn{
		ret.Neg(ret)
	}
	return ret
}

func testValidityOfTriples(t *testing.T) {
	for idx, PartyToTriples := range TS.Stock {
		for i := 0; i < len(PartyToTriples[1]); i++ {
			aShareSum, bShareSum, cShareSum := big.NewInt(0), big.NewInt(0), big.NewInt(0)
			for partyId := uint32(1); partyId <= uint32(len(PartyToTriples)); partyId++ {
				aShareSum.Add(aShareSum, convertToBigInt(PartyToTriples[partyId][i].A))
				bShareSum.Add(bShareSum, convertToBigInt(PartyToTriples[partyId][i].B))
				cShareSum.Add(cShareSum, convertToBigInt(PartyToTriples[partyId][i].C))
			}
			ab := big.NewInt(0)
			ab.Mul(aShareSum, bShareSum)
			if ab.Cmp(cShareSum) != 0 {
				t.Fatal("a*b != c")
			}
		}
	}
}

func testParallelGetTriples(t *testing.T, jobNum uint32, amount uint32, requestTime uint32) {
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
			t.Run("TestTripleGenerator", func(t *testing.T){
				t.Parallel()
				multiGetTriples(t, jobId, partyId, amount, requestTime)
			})
		}
	}

	t.Cleanup(func(){
		testValidityOfTriples(t)
	})
}

// --- 以下呼ばれる関数群 ---
func TestParallelGetTriples_1_1_1(t *testing.T){
	testParallelGetTriples(t, 1, 1, 1)
}
func TestParallelGetTriples_10_10_10(t *testing.T){
	testParallelGetTriples(t, 10, 10, 10)
}
func TestParallelGetTriples_10000_5_5(t *testing.T){
	testParallelGetTriples(t, 10000, 5, 5)
}
func TestParallelGetTriples_5_10000_5(t *testing.T){
	testParallelGetTriples(t, 5, 10000, 5)
}
func TestParallelGetTriples_5_5_10000(t *testing.T){
	testParallelGetTriples(t, 5, 5, 10000)
}

// 同じ request ID は同じ Triple
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
	for partyId := uint32(1); partyId <= uint32(len(claims.PartyInfo)); partyId++ {
		triples1, err := tg.GetTriples(claims, jobId, partyId, amount, 1)
		if err != nil {
			t.Fatal(err)
		}
		triples2, err := tg.GetTriples(claims, jobId, partyId, amount, 1)
		if err != nil {
			t.Fatal(err)
		}
		for i := uint32(0); i < amount; i++ {
			if triples1[i] != triples2[i] {
				t.Fatal("same requestId different triples")
			}
		}
	}
}

// 異なる request ID は異なる Triple
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
	for partyId := uint32(1); partyId <= uint32(len(claims.PartyInfo)); partyId++ {
		triples1, err := tg.GetTriples(claims, jobId, partyId, amount, 1)
		if err != nil {
			t.Fatal(err)
		}
		triples2, err := tg.GetTriples(claims, jobId, partyId, amount, 2)
		if err != nil {
			t.Fatal(err)
		}
		for i := uint32(0); i < amount; i++ {
			if triples1[i] == triples2[i] {
				t.Fatal("different requestId same triples")
			}
		}
	}
}

// 範囲外の PartyId が来た時にエラーを吐くか
func TestOutRangePartyId(t *testing.T){
	expected_text := "out range partyId"
	claims, err := getClaims()
	if err != nil {
		t.Fatal(err)
	}

	partyId := uint32(0)
	if _, err := tg.GetTriples(claims, 1, partyId, 1, -1); err.Error() != expected_text{
		t.Fatal("does not output 'out range partyId'")
	}

	partyId = uint32(len(claims.PartyInfo))  + uint32(1)
	if _, err := tg.GetTriples(claims, 1, partyId, 1, -1); err.Error() != expected_text{
		t.Fatal("does not output 'out range partyId'")
	}
}