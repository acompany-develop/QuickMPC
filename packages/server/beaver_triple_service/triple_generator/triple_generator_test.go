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
func multiGetTriples(t *testing.T, jobId uint32, partyId uint32, amount uint32, triple_type pb.Type, requestTimes uint32) {
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
			requestId := loopRequestId
			triples, err := tg.GetTriples(claims, jobId, partyId, amount, triple_type, int64(requestId))
			if err != nil {
				t.Fatal(err)
			}

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

func testValidityOfTriples(t *testing.T) {
	for _, PartyToTriples := range TS.Stock {
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


func parallelGetTriples(t *testing.T, jobNum uint32, amount uint32, triple_type pb.Type, requestTime uint32) {
	t.Helper()

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
				multiGetTriples(t, jobId, partyId, amount, triple_type, requestTime)
			})
		}
	}

	t.Cleanup(func(){
		testValidityOfTriples(t)

		// 初期化
		TS.Stock = make(map[uint32](map[uint32]([]*ts.Triple)))
		//tg.Db.ResetInstance()
		tg.Db = &ts.SafeTripleStore{
			Triples: make(map[uint32](map[uint32]([]*ts.Triple))),
			PreID: make(map[uint32](map[uint32](int64))),
			PreAmount: make(map[uint32](map[uint32](uint32))),
		}
	})
}

func testParallelGetTriples_FP(t *testing.T, jobNum uint32, amount uint32, requestTime uint32){
	parallelGetTriples(t, jobNum, amount, pb.Type_TYPE_FIXEDPOINT, requestTime)
}

func testParallelGetTriples_Float(t *testing.T, jobNum uint32, amount uint32, requestTime uint32){
	parallelGetTriples(t, jobNum, amount, pb.Type_TYPE_FLOAT, requestTime)
}

// --- 以下呼ばれる関数群 ---

func TestParallelGetTriples_FP_1_1_1(t *testing.T){
	testParallelGetTriples_FP(t, 1, 1, 1)
}
func TestParallelGetTriples_FP_10_10_10(t *testing.T){
	testParallelGetTriples_FP(t, 10, 10, 10)
}

func TestParallelGetTriples_Float_1_1_1(t *testing.T){
	testParallelGetTriples_Float(t, 1, 1, 1)
}
func TestParallelGetTriples_Float_10_10_10(t *testing.T){
	testParallelGetTriples_Float(t, 10, 10, 10)
}

func TestSameRequestId(t *testing.T){
	t.Helper()
	claims, err := getClaims()
	if err != nil {
		t.Fatal(err)
	}

	// jobId が他の Test と被らないようにする
	jobId := uint32(12345678)
	amount := uint32(1000)
	triple_type := pb.Type_TYPE_FIXEDPOINT
	for partyId := uint32(1); partyId <= uint32(len(claims.PartyInfo)); partyId++ {
		triples1, err := tg.GetTriples(claims, jobId, partyId, amount, triple_type, 1)
		if err != nil {
			t.Fatal(err)
		}
		triples2, err := tg.GetTriples(claims, jobId, partyId, amount, triple_type, 1)
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

func TestDifferentRequestId(t *testing.T){
	t.Helper()
	claims, err := getClaims()
	if err != nil {
		t.Fatal(err)
	}

	// jobId が他の Test と被らないようにする
	jobId := uint32(87654321)
	amount := uint32(1000)
	triple_type := pb.Type_TYPE_FIXEDPOINT
	for partyId := uint32(1); partyId <= uint32(len(claims.PartyInfo)); partyId++ {
		triples1, err := tg.GetTriples(claims, jobId, partyId, amount, triple_type, 1)
		if err != nil {
			t.Fatal(err)
		}
		triples2, err := tg.GetTriples(claims, jobId, partyId, amount, triple_type, 2)
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