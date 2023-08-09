package e2bserver

import (
	"context"
	"fmt"
	"math/big"
	"os"
	"sync"
	"testing"

	jwt_types "github.com/acompany-develop/QuickMPC/packages/server/beaver_triple_service/jwt"
	rbs "github.com/acompany-develop/QuickMPC/packages/server/beaver_triple_service/rand_bit_store"
	ts "github.com/acompany-develop/QuickMPC/packages/server/beaver_triple_service/triple_store"
	utils "github.com/acompany-develop/QuickMPC/packages/server/beaver_triple_service/utils"
	pb_types "github.com/acompany-develop/QuickMPC/proto/common_types"
	pb "github.com/acompany-develop/QuickMPC/proto/engine_to_bts"

	"google.golang.org/grpc/metadata"
)

type partyIdConuter struct {
	partyId uint32
	mux     sync.Mutex
}

var Pic partyIdConuter
var DbTripleTest *ts.SafeTripleStore
var DbRandBitTest *rbs.SafeRandBitStore

// Test用のサーバを起動(CC)
var s *utils.TestServer

func init() {
	// モック用GetPartyIdFromIp
	GetPartyIdFromClaims = func(claims *jwt_types.Claim) (uint32, error) {
		Pic.mux.Lock()
		defer Pic.mux.Unlock()

		if Pic.partyId++; Pic.partyId > uint32(len(claims.PartyInfo)) {
			Pic.partyId = 1
		}
		return Pic.partyId, nil
	}

	DbTripleTest = &ts.SafeTripleStore{Triples: make(map[uint32](map[uint32]([]*ts.Triple)))}
	DbRandBitTest = &rbs.SafeRandBitStore{RandBits: make(map[uint32](map[uint32]([]int64)))}

	s = &utils.TestServer{}
	pb.RegisterEngineToBtsServer(s.GetServer(), &server{})
	s.Serve()
}

func getClaims() (*jwt_types.Claim, error) {
	token, ok := os.LookupEnv("BTS_TOKEN")
	if ok {
		claims, err := utils.AuthJWT(token)
		if err != nil {
			return nil, err
		}

		return claims, nil
	}

	return nil, fmt.Errorf("BTS TOKEN is not valified")
}

type ctxOptFunc func(context.Context) context.Context

func withToken(token string) ctxOptFunc {
	return func(ctx context.Context) context.Context {
		md := metadata.New(map[string]string{"authorization": "bearer " + token})
		return metadata.NewOutgoingContext(ctx, md)
	}
}

func getContext(opts ...ctxOptFunc) (context.Context, error) {
	ctx := context.Background()

	// default値の設定
	token, ok := os.LookupEnv("BTS_TOKEN")
	if !ok {
		return nil, fmt.Errorf("BTS TOKEN is not valified")
	}
	ctx = withToken(token)(ctx)

	// optionの設定
	for _, opt := range opts {
		ctx = opt(ctx)
	}

	return ctx, nil
}

func testGetTriplesByJobIdAndPartyId(t *testing.T, client pb.EngineToBtsClient, amount uint32, jobId uint32, partyId uint32) {
	t.Run(fmt.Sprintf("testGetTriples_Party%d", partyId), func(t *testing.T) {
		t.Helper()
		t.Parallel()

		ctx, err := getContext()
		if err != nil {
			t.Fatal(err)
		}

		result, err := client.GetTriples(ctx, &pb.GetRequest{JobId: jobId, Amount: amount, RequestId: -1})
		if err != nil {
			t.Fatal(err)
		}

		DbTripleTest.Mux.Lock()
		if DbTripleTest.Triples[jobId][partyId] != nil {
			DbTripleTest.Mux.Unlock()
			t.Fatal("すでに同じTripleが存在")
		}

		if len(DbTripleTest.Triples[jobId]) == 0 {
			DbTripleTest.Triples[jobId] = make(map[uint32]([]*ts.Triple))
		}
		DbTripleTest.Triples[jobId][partyId] = result.Triples
		DbTripleTest.Mux.Unlock()
	})
}

func testGetTriplesByJobId(t *testing.T, client pb.EngineToBtsClient, amount uint32, jobId uint32) {
	claims, err := getClaims()
	if err != nil {
		t.Fatal(err)
	}

	t.Run(fmt.Sprintf("testGetTriples_Job%d", jobId), func(t *testing.T) {
		t.Helper()
		for partyId := uint32(1); partyId <= uint32(len(claims.PartyInfo)); partyId++ {
			partyId := partyId
			testGetTriplesByJobIdAndPartyId(t, client, amount, jobId, partyId)
		}
	})
}

func convertToBigInt(b *pb_types.BigIntByte) *big.Int {
	var ret *big.Int = big.NewInt(0)
	bytes := b.AbsByte
	ret.SetBytes(bytes)
	if b.Sgn {
		ret.Neg(ret)
	}
	return ret
}

func testValidityOfTriples(t *testing.T) {
	for _, triples := range DbTripleTest.Triples {
		for i := 0; i < len(triples[1]); i++ {
			aShareSum, bShareSum, cShareSum := big.NewInt(0), big.NewInt(0), big.NewInt(0)
			for partyId := uint32(1); partyId <= uint32(len(triples)); partyId++ {
				aShareSum.Add(aShareSum, convertToBigInt(triples[partyId][i].A))
				bShareSum.Add(bShareSum, convertToBigInt(triples[partyId][i].B))
				cShareSum.Add(cShareSum, convertToBigInt(triples[partyId][i].C))
			}
			ab := big.NewInt(0)
			ab.Mul(aShareSum, bShareSum)
			if ab.Cmp(cShareSum) != 0 {
				t.Fatal("a*b != c")
			}
		}
	}
}

func testGetTriples(t *testing.T, amount uint32, jobNum uint32) {
	t.Run("TestGetTriple", func(t *testing.T) {
		conn := s.GetConn()
		defer conn.Close()
		client := pb.NewEngineToBtsClient(conn)

		for jobId := uint32(0); jobId < jobNum; jobId++ {
			jobId := jobId
			testGetTriplesByJobId(t, client, amount, jobId)
		}
	})

	t.Run("TestValidity", func(t *testing.T) {
		testValidityOfTriples(t)
		DbTripleTest.Triples = make(map[uint32](map[uint32]([]*ts.Triple)))
	})
}

func TestGetTriples_1_1(t *testing.T)     { testGetTriples(t, 1, 1) }     // 0.008s
func TestGetTriples_1_100(t *testing.T)   { testGetTriples(t, 1, 100) }   // 0.05s
func TestGetTriples_1_10000(t *testing.T) { testGetTriples(t, 1, 10000) } // 8.0s

func TestGetTriples_100_1(t *testing.T)   { testGetTriples(t, 100, 1) }   // 0.01s
func TestGetTriples_100_100(t *testing.T) { testGetTriples(t, 100, 100) } // 0.17s
// func TestGetTriples_100_10000(t *testing.T) { testGetTriples(t, 100, 10000) } // 17s

func TestGetTriples_10000_1(t *testing.T)   { testGetTriples(t, 10000, 1) }   // 0.1s
func TestGetTriples_10000_100(t *testing.T) { testGetTriples(t, 10000, 100) } // 10s
// func TestGetTriples_10000_10000(t *testing.T) { testGetTriples(t, 10000, 10000) } // TO(10分以上)

// --- RandBit --- ファイル分けても良いかも

func testGetRandBitsByJobIdAndPartyId(t *testing.T, client pb.EngineToBtsClient, amount uint32, jobId uint32, partyId uint32) {
	t.Run(fmt.Sprintf("testGetRandBits_Party%d", partyId), func(t *testing.T) {
		t.Helper()
		t.Parallel()

		ctx, err := getContext()
		if err != nil {
			t.Fatal(err)
		}

		result, err := client.GetRandBits(ctx, &pb.GetRequest{JobId: jobId, Amount: amount, RequestId: -1})
		if err != nil {
			t.Fatal(err)
		}

		DbRandBitTest.Mux.Lock()
		if DbRandBitTest.RandBits[jobId][partyId] != nil {
			DbRandBitTest.Mux.Unlock()
			t.Fatal("すでに同じRandBitが存在")
		}

		if len(DbRandBitTest.RandBits[jobId]) == 0 {
			DbRandBitTest.RandBits[jobId] = make(map[uint32]([]int64))
		}
		DbRandBitTest.RandBits[jobId][partyId] = result.Randbits
		DbRandBitTest.Mux.Unlock()
	})
}

func testGetRandBitsByJobId(t *testing.T, client pb.EngineToBtsClient, amount uint32, jobId uint32) {
	claims, err := getClaims()
	if err != nil {
		t.Fatal(err)
	}

	t.Run(fmt.Sprintf("testGetRandBits_Job%d", jobId), func(t *testing.T) {
		t.Helper()
		for partyId := uint32(1); partyId <= uint32(len(claims.PartyInfo)); partyId++ {
			partyId := partyId
			testGetRandBitsByJobIdAndPartyId(t, client, amount, jobId, partyId)
		}
	})
}

func testValidityOfRandBits(t *testing.T) {
	for _, randbits := range DbRandBitTest.RandBits {
		for i := 0; i < len(randbits[1]); i++ {
			bitSum := int64(0)
			for partyId := uint32(1); partyId <= uint32(len(randbits)); partyId++ {
				bitSum += randbits[partyId][i]
			}
			if bitSum != 0 && bitSum != 1 {
				t.Fatal("This is not bit")
			}
		}
	}
}

func testGetRandBits(t *testing.T, amount uint32, jobNum uint32) {
	t.Run("TestGetTriple", func(t *testing.T) {
		conn := s.GetConn()
		defer conn.Close()
		client := pb.NewEngineToBtsClient(conn)

		for jobId := uint32(0); jobId < jobNum; jobId++ {
			jobId := jobId
			testGetRandBitsByJobId(t, client, amount, jobId)
		}
	})

	t.Run("TestValidity", func(t *testing.T) {
		testValidityOfRandBits(t)
		DbRandBitTest.RandBits = make(map[uint32](map[uint32]([]int64)))
	})
}

func TestGetRandBits_1_1(t *testing.T)     { testGetRandBits(t, 1, 1) }
func TestGetRandBits_1_100(t *testing.T)   { testGetRandBits(t, 1, 100) }
func TestGetRandBits_1_10000(t *testing.T) { testGetRandBits(t, 1, 10000) }

func TestGetRandBits_100_1(t *testing.T)   { testGetRandBits(t, 100, 1) }
func TestGetRandBits_100_100(t *testing.T) { testGetRandBits(t, 100, 100) }

// func TestGetRandBits_100_10000(t *testing.T) { testGetRandBits(t, 100, 10000) }

func TestGetRandBits_10000_1(t *testing.T)   { testGetRandBits(t, 10000, 1) }
func TestGetRandBits_10000_100(t *testing.T) { testGetRandBits(t, 10000, 100) }

// func TestGetRandBits_10000_10000(t *testing.T) { testGetRandBits(t, 10000, 10000) }
