package triplegenerator_test

import (
	"context"
	"os"
	"fmt"
	"testing"

	jwt_types "github.com/acompany-develop/QuickMPC/packages/server/beaver_triple_service/jwt"
	utils "github.com/acompany-develop/QuickMPC/packages/server/beaver_triple_service/utils"
	tg "github.com/acompany-develop/QuickMPC/packages/server/beaver_triple_service/triple_generator"
	ts "github.com/acompany-develop/QuickMPC/packages/server/beaver_triple_service/triple_store"
	pb "github.com/acompany-develop/QuickMPC/proto/engine_to_bts"
)

var Db *ts.SafeTripleStore
var DbTest *ts.SafeTripleStore

func init() {
	Db = ts.GetInstance()
	DbTest = &ts.SafeTripleStore{Triples: make(map[uint32](map[uint32]([]*ts.Triple)))}
}

func getContext() (context.Context, error){
	token, ok := os.LookupEnv("BTS_TOKEN")
	if ok {
		claims,err := utils.AuthJWT(token)
		if err != nil {
			return nil,err
		}
		ctx := context.Background()
		ctx = context.WithValue(ctx, "claims", claims)
		return ctx,nil
	}

	return nil,fmt.Errorf("BTS TOKEN is not valified")
}

func getTriplesForParallel(t *testing.T, partyId uint32, amount uint32, jobNum uint32, triple_type pb.Type) {
	ctx, err := getContext()
	if err != nil {
		t.Fatal(err)
	}

	t.Helper()
	for jobId := uint32(0); jobId < jobNum; jobId++ {
		t.Run(fmt.Sprintf("TestTripleGenerator_Job%d", jobId), func(t *testing.T) {
			triples, err := tg.GetTriples(ctx, jobId, partyId, amount, triple_type)
			if err != nil {
				t.Fatal(err)
			}

			DbTest.Mux.Lock()
			if DbTest.Triples[jobId][partyId] != nil {
				DbTest.Mux.Unlock()
				t.Fatal("すでに同じTripleが存在")
			}

			if len(DbTest.Triples[jobId]) == 0 {
				DbTest.Triples[jobId] = make(map[uint32]([]*ts.Triple))
			}
			DbTest.Triples[jobId][partyId] = triples
			DbTest.Mux.Unlock()
		})
	}
}

func testDbIsEmpty(t *testing.T) {
	count := 0
	for _, t := range Db.Triples {
		count += len(t)
	}
	if count != 0 {
		t.Log(Db.Triples)
		t.Fatal("残存Tripleあり")
	}
}

func testValidityOfTriples(t *testing.T) {
	for _, triples := range DbTest.Triples {
		for i := 0; i < len(triples[1]); i++ {
			aShareSum, bShareSum, cShareSum := int64(0), int64(0), int64(0)
			for partyId := uint32(1); partyId <= uint32(len(triples)); partyId++ {
				aShareSum += triples[partyId][i].A
				bShareSum += triples[partyId][i].B
				cShareSum += triples[partyId][i].C
			}
			if aShareSum*bShareSum != cShareSum {
				t.Fatal("a*b != c")
			}
		}
	}
}

func testTripleGenerator(t *testing.T, amount uint32, jobNum uint32, triple_type pb.Type) {
	t.Helper()

	t.Run("TestTripleGenerator", func(t *testing.T) {
		ctx, err := getContext()
		if err != nil {
			t.Fatal(err)
		}

		claims, _ := ctx.Value("claims").(*jwt_types.Claim)

		for partyId := uint32(1); partyId <= claims.PartyNum; partyId++ {
			// NOTE: https://github.com/golang/go/wiki/CommonMistakes#using-goroutines-on-loop-iterator-variables
			partyId := partyId
			t.Run(fmt.Sprintf("TestTripleGenerator_Party%d", partyId), func(t *testing.T) {
				t.Parallel()
				getTriplesForParallel(t, partyId, amount, jobNum, triple_type)
			})
		}
	})
	t.Run("TestValidity", func(t *testing.T) {
		testDbIsEmpty(t)
		testValidityOfTriples(t)
		DbTest.Triples = make(map[uint32](map[uint32]([]*ts.Triple)))
	})
}

func TestTripleGenerator_1_1(t *testing.T)   { testTripleGenerator(t, 1, 1, pb.Type_TYPE_FIXEDPOINT) }   // 0s
func TestTripleGenerator_1_100(t *testing.T) { testTripleGenerator(t, 1, 100, pb.Type_TYPE_FIXEDPOINT) } // 0.014s
func TestTripleGenerator_1_10000(t *testing.T) {
	testTripleGenerator(t, 1, 10000, pb.Type_TYPE_FIXEDPOINT)
} // 5.0s

func TestTripleGenerator_100_1(t *testing.T) { testTripleGenerator(t, 100, 1, pb.Type_TYPE_FIXEDPOINT) } // 0.004s
func TestTripleGenerator_100_100(t *testing.T) {
	testTripleGenerator(t, 100, 100, pb.Type_TYPE_FIXEDPOINT)
} // 0.12s
func TestTripleGenerator_100_10000(t *testing.T) {
	testTripleGenerator(t, 100, 10000, pb.Type_TYPE_FIXEDPOINT)
} // 14s

func TestTripleGenerator_10000_1(t *testing.T) {
	testTripleGenerator(t, 10000, 1, pb.Type_TYPE_FIXEDPOINT)
} // 0.10s
func TestTripleGenerator_10000_100(t *testing.T) {
	testTripleGenerator(t, 10000, 100, pb.Type_TYPE_FIXEDPOINT)
} // 9.3s
// func TestTripleGenerator_10000_10000(t *testing.T) { testTripleGenerator(t, 10000, 10000,pb.Type_TYPE_FIXEDPOINT) } // TO(10分以上)

func TestTripleGenerator_1000000_1(t *testing.T) {
	testTripleGenerator(t, 1000000, 1, pb.Type_TYPE_FIXEDPOINT)
} // 10s
// func TestTripleGenerator_1000000_100(t *testing.T) { testTripleGenerator(t, 1000000, 100,pb.Type_TYPE_FIXEDPOINT) } // TO(10分以上)

func TestTripleGenerator_Float_1_1(t *testing.T) { testTripleGenerator(t, 1, 1, pb.Type_TYPE_FLOAT) }
func TestTripleGenerator_Float_1_100(t *testing.T) {
	testTripleGenerator(t, 1, 100, pb.Type_TYPE_FLOAT)
}
func TestTripleGenerator_Float_1_10000(t *testing.T) {
	testTripleGenerator(t, 1, 10000, pb.Type_TYPE_FLOAT)
}
func TestTripleGenerator_Float_100_1(t *testing.T) {
	testTripleGenerator(t, 100, 1, pb.Type_TYPE_FLOAT)
}
func TestTripleGenerator_Float_100_100(t *testing.T) {
	testTripleGenerator(t, 100, 100, pb.Type_TYPE_FLOAT)
}
func TestTripleGenerator_Float_100_10000(t *testing.T) {
	testTripleGenerator(t, 100, 10000, pb.Type_TYPE_FLOAT)
}
func TestTripleGenerator_Float_10000_1(t *testing.T) {
	testTripleGenerator(t, 10000, 1, pb.Type_TYPE_FLOAT)
}
func TestTripleGenerator_Float_10000_100(t *testing.T) {
	testTripleGenerator(t, 10000, 100, pb.Type_TYPE_FLOAT)
}
func TestTripleGenerator_Float_1000000_1(t *testing.T) {
	testTripleGenerator(t, 1000000, 1, pb.Type_TYPE_FLOAT)
}

func TestInitTripleStore(t *testing.T) {
	ctx, err := getContext()
	if err != nil {
		t.Fatal(err)
	}

	jobId := uint32(1)
	partyId := uint32(1)
	amount := uint32(10)
	triple_type := pb.Type_TYPE_FLOAT
	_, err := tg.GetTriples(ctx, jobId, partyId, amount, triple_type)
	if err != nil {
		t.Fatal(err)
	}

	err = tg.InitTripleStore()
	if err != nil {
		t.Fatal(err)
	}

	testDbIsEmpty(t)
}

func TestDeleteJobIdTriple(t *testing.T) {
	ctx, err := getContext()
	if err != nil {
		t.Fatal(err)
	}

	jobNum := uint32(10)
	partyId := uint32(1)
	amount := uint32(10)
	triple_type := pb.Type_TYPE_FLOAT
	for jobId := uint32(0); jobId < jobNum; jobId++ {
		_, err := tg.GetTriples(ctx, jobId, partyId, amount, triple_type)
		if err != nil {
			t.Fatal(err)
		}
	}

	for jobId := uint32(0); jobId < jobNum; jobId++ {
		err := tg.DeleteJobIdTriple(jobId)
		if err != nil {
			t.Fatal(err)
		}

		if len(Db.Triples[jobId]) != 0 {
			t.Log(Db.Triples[jobId])
			t.Fatal("残存Tripleあり")
		}
	}
}
