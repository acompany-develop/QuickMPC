package randbitstore_test

import (
	"os"
	"fmt"
	jwt_types "github.com/acompany-develop/QuickMPC/packages/server/beaver_triple_service/jwt"
	utils "github.com/acompany-develop/QuickMPC/packages/server/beaver_triple_service/utils"
	rbs "github.com/acompany-develop/QuickMPC/packages/server/beaver_triple_service/rand_bit_store"
	"testing"
)

var Db *rbs.SafeRandBitStore

func init() {
	Db = rbs.GetInstance()
}

func getClaims() (*jwt_types.Claim, error) {
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

func generateRandBits(amount uint32) map[uint32]([]int64) {
	ret := make(map[uint32]([]int64))
	for i := uint32(0); i < amount; i++ {
		b1 := int64(10)
		ret[1] = append(ret[1], b1)

		b2 := int64(-10)
		ret[2] = append(ret[2], b2)

		b3 := int64(i % 2) - b1 - b2
		ret[3] = append(ret[3], b3)
	}

	return ret
}

func getRandBits(t *testing.T, jobId uint32, partyId uint32, amount uint32) []int64 {
	Db.Mux.Lock()
	defer Db.Mux.Unlock()

	if len(Db.RandBits[jobId]) == 0 {
		Db.RandBits[jobId] = generateRandBits(amount)
	}

	randbits, ok := Db.RandBits[jobId][partyId]
	if ok {
		delete(Db.RandBits[jobId], partyId)
	}

	if len(randbits) == 0 {
		t.Fatal("すでに取得済みのリソースがリクエストされた")
	}
	return randbits
}

func getRandBitsForParallel(t *testing.T, partyId uint32, amount uint32, jobNum uint32) {
	t.Helper()
	for jobId := uint32(0); jobId < jobNum; jobId++ {
		t.Run(fmt.Sprintf("TestRandBitStore_Job%d", jobId), func(t *testing.T) {
			getRandBits(t, jobId, partyId, amount)
		})
	}
}

func testDbIsEmpty(t *testing.T) {
	count := 0
	for _, t := range Db.RandBits {
		count += len(t)
	}
	if count != 0 {
		t.Log(Db.RandBits)
		t.Fatal("残存RandBitあり")
	}
}

func testRandBitStore(t *testing.T, amount uint32, jobNum uint32) {
	t.Helper()

	t.Run("TestRandBitStore", func(t *testing.T) {
		claims, err := getClaims()
		if err != nil {
			t.Fatal(err)
		}

		for partyId := uint32(1); partyId <= uint32(len(claims.PartyInfo)); partyId++ {
			// NOTE: https://github.com/golang/go/wiki/CommonMistakes#using-goroutines-on-loop-iterator-variables
			partyId := partyId
			t.Run(fmt.Sprintf("TestRandBitStore_Party%d", partyId), func(t *testing.T) { t.Parallel(); getRandBitsForParallel(t, partyId, amount, jobNum) })
		}
	})
	t.Run("TestValidity", func(t *testing.T) {
		testDbIsEmpty(t)
	})
}

func TestRandBitStore_1_1(t *testing.T)     { testRandBitStore(t, 1, 1) }
func TestRandBitStore_1_100(t *testing.T)   { testRandBitStore(t, 1, 100) }
func TestRandBitStore_1_10000(t *testing.T) { testRandBitStore(t, 1, 10000) }

func TestRandBitStore_100_1(t *testing.T)     { testRandBitStore(t, 100, 1) }
func TestRandBitStore_100_100(t *testing.T)   { testRandBitStore(t, 100, 100) }
func TestRandBitStore_100_10000(t *testing.T) { testRandBitStore(t, 100, 10000) }

func TestRandBitStore_10000_1(t *testing.T)   { testRandBitStore(t, 10000, 1) }
func TestRandBitStore_10000_100(t *testing.T) { testRandBitStore(t, 10000, 100) }
// func TestRandBitStore_10000_10000(t *testing.T) { testRandBitStore(t, 10000, 10000) }

func TestRandBitStore_1000000_1(t *testing.T) { testRandBitStore(t, 1000000, 1) }
// func TestRandBitStore_1000000_100(t *testing.T) { testRandBitStore(t, 1000000, 100) }
