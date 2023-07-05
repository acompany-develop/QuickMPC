package triplegenerator_test

import (
	"os"
	"fmt"
	"testing"
	"sync"

	jwt_types "github.com/acompany-develop/QuickMPC/packages/server/beaver_triple_service/jwt"
	utils "github.com/acompany-develop/QuickMPC/packages/server/beaver_triple_service/utils"
	tg "github.com/acompany-develop/QuickMPC/packages/server/beaver_triple_service/triple_generator"
	ts "github.com/acompany-develop/QuickMPC/packages/server/beaver_triple_service/triple_store"
	pb "github.com/acompany-develop/QuickMPC/proto/engine_to_bts"
)

var Db *ts.SafeTripleStore

type TriplesStock struct{
	Stock map[uint32](map[uint32]([]*ts.Triple))
	Mux     sync.Mutex
}
var TS TriplesStock

func init() {
	TS.Stock = make(map[uint32](map[uint32]([]*ts.Triple)))
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
func multiGetTriples(t *testing.T, jobId uint32, partyId uint32, amount uint32, requestTimes uint32, triple_type pb.Type) {
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
		for requestId := uint32(0); requestId < requestTimes; requestId++ {
			triples, err := tg.GetTriples(claims, jobId, partyId, amount, triple_type, requestId)
			if err != nil {
				t.Fatal(err)
			}

			TS.Mux.Lock()

			_, ok := TS.Stock[jobId][PartyId]
			if ok {
				TS.Stock[jobId][PartyId] = append(TS.Stock[jobId][PartyId], triples)
			} else {
				Db.Triples[jobId][partyId] = triples
			}

			TS.Mux.Unlock()
		}
	})
}

func testValidityOfTriples(t *testing.T) {
	for jobId, PartyToTriples := range TS.Stock {
		for i := 0; i < len(PartyToTriples[1]); i++ {
			aShareSum, bShareSum, cShareSum := int64(0), int64(0), int64(0)
			for partyId := uint32(1); partyId <= uint32(len(PartyToTriples)); partyId++ {
				aShareSum += PartyToTriples[partyId][i].A
				bShareSum += PartyToTriples[partyId][i].B
				cShareSum += PartyToTriples[partyId][i].C
			}
			if aShareSum*bShareSum != cShareSum {
				t.Fatal("a*b != c")
			}
		}
	}
}

func parallelGetTriples(t *testing.T, amount uint32, jobNum uint32, requestTimes uint32, triple_type pb.Type) {
	t.Helper()

	t.Run("TestTripleGenerator", func(t *testing.T) {
		claims, err := getClaims()
		if err != nil {
			t.Fatal(err)
		}

		for jobId := 0; jobId < jobNum; jobId++ {
			for partyId := 1; partyId <= uint32(len(claims.PartyInfo)); jobId++ {
				t.Parallel();
				multiGetTriples(t, jobId, partyId, amount, requestTimes, triple_type)
			}
		}
	})

	t.Run("TestValidity", func(t *testing.T) {
		testValidityOfTriples(t)
		TS.Stock = make(map[uint32](map[uint32]([]*ts.Triple)))
	})
}

func testParallelGetTriples_FP(t *testing.T, amount uint32, jobNum uint32, requestTimes uint32){
	parallelGetTriples(t *testing.T, amount, jobNum, requestTimes, pb.Type_TYPE_FIXEDPOINT)
}

func testParallelGetTriples_Float(t *testing.T, amount uint32, jobNum uint32, requestTimes uint32){
	parallelGetTriples(t *testing.T, amount, jobNum, requestTimes, pb.Type_TYPE_FLOAT)
}

// --- 以下呼ばれる関数群 ---

func TestParallelGetTriples_FP_1_1_1(t *testing.T){
	testParallelGetTriples_FP(t, 1, 1, 1)
}
func TestParallelGetTriples_FP_10_10_10(t *testing.T){
	testParallelGetTriples_FP(t, 10, 10, 10)
}
func TestParallelGetTriples_FP_100_100_100(t *testing.T){
	testParallelGetTriples_FP(t, 100, 100, 100)
}

func TestParallelGetTriples_Float_1_1_1(t *testing.T){
	testParallelGetTriples_Float(t, 1, 1, 1)
}
func TestParallelGetTriples_Float_10_10_10(t *testing.T){
	testParallelGetTriples_Float(t, 10, 10, 10)
}
func TestParallelGetTriples_Float_100_100_100(t *testing.T){
	testParallelGetTriples_Float(t, 100, 100, 100)
}

func TestSameRequestId(t *testing.T){
	t.Helper()

	claims, err := getClaims()
	if err != nil {
		t.Fatal(err)
	}

	// jobId が他の Test と被らないようにする
	jobId := 12345678

	for partyId := uint32(1); partyId <= uint32(len(PartyToTriples)); partyId++ {
		triples1, err := tg.GetTriples(claims, jobId, partyId, amount, triple_type, 1)
		if err != nil {
			t.Fatal(err)
		}
		triples2, err := tg.GetTriples(claims, jobId, partyId, amount, triple_type, 1)
		if err != nil {
			t.Fatal(err)
		}
		if triples1 != triples2 {
			t.Fatal("same requestId different triples")
		}
	}
}

func TestDifferentRequestId(t *testing.T){
	t.Helper()

	claims, err := getClaims()
	if err != nil {
		t.Fatal(err)
	}

	// jobId が他の Test と被らないようにする
	jobId := 87654321

	for partyId := uint32(1); partyId <= uint32(len(PartyToTriples)); partyId++ {
		triples1, err := tg.GetTriples(claims, jobId, partyId, amount, triple_type, 1)
		if err != nil {
			t.Fatal(err)
		}
		triples2, err := tg.GetTriples(claims, jobId, partyId, amount, triple_type, 2)
		if err != nil {
			t.Fatal(err)
		}
		if triples1 == triples2 {
			t.Fatal("different requestId same triples")
		}
	}
}