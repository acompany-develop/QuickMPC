package triplestore_test

import (
	"fmt"
	cs "github.com/acompany-develop/QuickMPC/packages/server/beaver_triple_service/config_store"
	ts "github.com/acompany-develop/QuickMPC/packages/server/beaver_triple_service/triple_store"
	"testing"
)

var Db *ts.SafeTripleStore

func init() {
	Db = ts.GetInstance()
}

func generateTriples(amount uint32) map[uint32]([]*ts.Triple) {
	ret := make(map[uint32]([]*ts.Triple))
	for i := uint32(0); i < amount; i++ {
		t := ts.Triple{
			A: 1,
			B: 1,
			C: 3,
		}
		ret[1] = append(ret[1], &t)

		t = ts.Triple{
			A: 2,
			B: 2,
			C: 6,
		}
		ret[2] = append(ret[2], &t)

		t = ts.Triple{
			A: 3,
			B: 3,
			C: 9,
		}
		ret[3] = append(ret[3], &t)
	}

	return ret
}

func getTriples(t *testing.T, jobId uint32, partyId uint32, amount uint32) []*ts.Triple {
	Db.Mux.Lock()
	defer Db.Mux.Unlock()

	if len(Db.Triples[jobId]) == 0 {
		Db.Triples[jobId] = generateTriples(amount)
	}

	triples, ok := Db.Triples[jobId][partyId]
	if ok {
		delete(Db.Triples[jobId], partyId)
	}

	if len(triples) == 0 {
		t.Fatal("すでに取得済みのリソースがリクエストされた")
	}
	return triples
}

func getTriplesForParallel(t *testing.T, partyId uint32, amount uint32, jobNum uint32) {
	t.Helper()
	for jobId := uint32(0); jobId < jobNum; jobId++ {
		t.Run(fmt.Sprintf("TestTripleStore_Job%d", jobId), func(t *testing.T) {
			getTriples(t, jobId, partyId, amount)
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

func testTripleStore(t *testing.T, amount uint32, jobNum uint32) {
	t.Helper()

	t.Run("TestTripleStore", func(t *testing.T) {
		for partyId := uint32(1); partyId <= cs.Conf.PartyNum; partyId++ {
			// NOTE: https://github.com/golang/go/wiki/CommonMistakes#using-goroutines-on-loop-iterator-variables
			partyId := partyId
			t.Run(fmt.Sprintf("TestTripleStore_Party%d", partyId), func(t *testing.T) { t.Parallel(); getTriplesForParallel(t, partyId, amount, jobNum) })
		}
	})
	t.Run("TestValidity", func(t *testing.T) {
		testDbIsEmpty(t)
	})
}

func TestTripleStore_1_1(t *testing.T)     { testTripleStore(t, 1, 1) }     // 0s
func TestTripleStore_1_100(t *testing.T)   { testTripleStore(t, 1, 100) }   // 0.012s
func TestTripleStore_1_10000(t *testing.T) { testTripleStore(t, 1, 10000) } // 4.5s

func TestTripleStore_100_1(t *testing.T)     { testTripleStore(t, 100, 1) }     // 0.003s
func TestTripleStore_100_100(t *testing.T)   { testTripleStore(t, 100, 100) }   // 0.015s
func TestTripleStore_100_10000(t *testing.T) { testTripleStore(t, 100, 10000) } // 4.5s

func TestTripleStore_10000_1(t *testing.T)   { testTripleStore(t, 10000, 1) }   // 0.004s
func TestTripleStore_10000_100(t *testing.T) { testTripleStore(t, 10000, 100) } // 0.15s
// func TestTripleStore_10000_10000(t *testing.T) { testTripleStore(t, 10000, 10000) } 	// 15s

func TestTripleStore_1000000_1(t *testing.T) { testTripleStore(t, 1000000, 1) } // 0.15s
// func TestTripleStore_1000000_100(t *testing.T) { testTripleStore(t, 1000000, 100) } 	// 10s
