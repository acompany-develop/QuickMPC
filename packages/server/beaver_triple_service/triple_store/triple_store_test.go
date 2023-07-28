package triplestore_test

import (
	"os"
	"fmt"
	"bytes"
	"encoding/binary"
	jwt_types "github.com/acompany-develop/QuickMPC/packages/server/beaver_triple_service/jwt"
	utils "github.com/acompany-develop/QuickMPC/packages/server/beaver_triple_service/utils"
	ts "github.com/acompany-develop/QuickMPC/packages/server/beaver_triple_service/triple_store"
	pb "github.com/acompany-develop/QuickMPC/proto/engine_to_bts"
	"testing"
)

var Db *ts.SafeTripleStore

func init() {
	Db = ts.GetInstance()
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

func convertToBigIntByte(a int64)(*pb.BigIntByte, error){
	sgn := bool(a < 0)
	buf := new(bytes.Buffer)
	err := binary.Write(buf, binary.BigEndian, a)
	if err != nil {
        return nil, err
    }
    byteSlice := buf.Bytes()
	return &pb.BigIntByte{
		Sgn : sgn,
		AbsByte : byteSlice,
	}, nil
}

func convertToTriple(a,b,c int64)(*pb.Triple, error){
	a_, err := convertToBigIntByte(a)
	if err != nil{
		return nil, err
	}
	b_, err := convertToBigIntByte(b)
	if err != nil{
		return nil, err
	}
	c_, err := convertToBigIntByte(c)
	if err != nil{
		return nil, err
	}
	return &pb.Triple{
		A : a_,
		B : b_,
		C : c_,
	}, nil
}

func generateTriples(amount uint32) (map[uint32]([]*ts.Triple), error) {
	ret := make(map[uint32]([]*ts.Triple))
	for i := uint32(0); i < amount; i++ {
		t, err := convertToTriple(1, 1, 3)
		if err != nil{
			return nil, err
		}
		ret[1] = append(ret[1], t)

		t, err = convertToTriple(2, 2, 6)
		if err != nil{
			return nil, err
		}
		ret[2] = append(ret[2], t)

		t, err = convertToTriple(3, 3, 9)
		if err != nil{
			return nil, err
		}
		ret[3] = append(ret[3], t)
	}

	return ret, nil
}

func getTriples(t *testing.T, jobId uint32, partyId uint32, amount uint32) ([]*ts.Triple, error){
	Db.Mux.Lock()
	defer Db.Mux.Unlock()

	if len(Db.Triples[jobId]) == 0 {
		newTriples, err := generateTriples(amount)
		if err != nil{
			return nil, err
		}
		Db.Triples[jobId] = newTriples
	}

	triples, ok := Db.Triples[jobId][partyId]
	if ok {
		delete(Db.Triples[jobId], partyId)
	}

	if len(triples) == 0 {
		t.Fatal("すでに取得済みのリソースがリクエストされた")
	}
	return triples, nil
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
		claims, err := getClaims()
		if err != nil {
			t.Fatal(err)
		}

		for partyId := uint32(1); partyId <= uint32(len(claims.PartyInfo)); partyId++ {
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
