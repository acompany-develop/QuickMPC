package e2bserver

import (
	"context"
	"encoding/base64"
	"errors"
	"fmt"
	"os"
	"strings"
	"sync"
	"testing"
	"time"

	cs "github.com/acompany-develop/QuickMPC/packages/server/BeaverTripleService/ConfigStore"
	ts "github.com/acompany-develop/QuickMPC/packages/server/BeaverTripleService/TripleStore"
	utils "github.com/acompany-develop/QuickMPC/packages/server/BeaverTripleService/Utils"
	pb "github.com/acompany-develop/QuickMPC/proto/EngineToBts"
	"github.com/golang-jwt/jwt/v4"
)

type partyIdConuter struct {
	partyId uint32
	mux     sync.Mutex
}

var Pic partyIdConuter
var DbTest *ts.SafeTripleStore

// Test用のサーバを起動(CC)
var s *utils.TestServer

func init() {
	// モック用GetPartyIdFromIp
	GetPartyIdFromIp = func(reqIpAddrAndPort string) (uint32, error) {
		if reqIpAddrAndPort == "bufconn" {
			Pic.mux.Lock()
			defer Pic.mux.Unlock()
			if Pic.partyId++; Pic.partyId > cs.Conf.PartyNum {
				Pic.partyId = 1
			}
		} else {
			return 0, fmt.Errorf("reqIpAddrAndPortがbufconnではない(%s)", reqIpAddrAndPort)
		}

		return Pic.partyId, nil
	}

	DbTest = &ts.SafeTripleStore{Triples: make(map[uint32](map[uint32]([]*ts.Triple)))}

	s = &utils.TestServer{}
	pb.RegisterEngineToBtsServer(s.GetServer(), &server{})
	s.Serve()
}

func testGetTriplesByJobIdAndPartyId(t *testing.T, client pb.EngineToBtsClient, amount uint32, jobId uint32, partyId uint32) {
	t.Run(fmt.Sprintf("testGetTriples_Party%d", partyId), func(t *testing.T) {
		t.Helper()
		t.Parallel()
		result, err := client.GetTriples(context.Background(), &pb.GetTriplesRequest{JobId: jobId, Amount: amount, TripleType: pb.Type_TYPE_FIXEDPOINT})
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
		DbTest.Triples[jobId][partyId] = result.Triples
		DbTest.Mux.Unlock()
	})
}

func testGetTriplesByJobId(t *testing.T, client pb.EngineToBtsClient, amount uint32, jobId uint32) {
	t.Run(fmt.Sprintf("testGetTriples_Job%d", jobId), func(t *testing.T) {
		t.Helper()
		for partyId := uint32(1); partyId <= cs.Conf.PartyNum; partyId++ {
			partyId := partyId
			testGetTriplesByJobIdAndPartyId(t, client, amount, jobId, partyId)
		}
	})
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
		DbTest.Triples = make(map[uint32](map[uint32]([]*ts.Triple)))
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

// TripleTypeを指定しない場合にエラーが出るかテスト
func TestGetTriplesFailedUnknownType(t *testing.T) {
	conn := s.GetConn()
	defer conn.Close()
	client := pb.NewEngineToBtsClient(conn)

	_, err := client.GetTriples(context.Background(), &pb.GetTriplesRequest{JobId: 0, Amount: 1})

	if err == nil {
		t.Fatal("TripleTypeの指定がないRequestはエラーを出す必要があります．")
	}
}

func TestAuthToken(t *testing.T) {
	type testCase struct {
		description string
		alg         jwt.SigningMethod
		claims      jwt.MapClaims
		expected    error
		encodeKey   string
		decodeKey   string
	}

	tommorow := time.Now().Add(time.Hour * 24).Unix()

	validClaim := jwt.MapClaims{
		"exp": tommorow,
	}

	merge := func(left, right jwt.MapClaims) jwt.MapClaims {
		// Go だと shallow copy になるので非破壊的なmergeをしたい場合は新しいmapを用意する
		m := jwt.MapClaims{}
		for key, value := range left {
			m[key] = value
		}
		for key, value := range right {
			m[key] = value
		}
		return m
	}

	testcases := []testCase{
		{
			description: "validなJWTにはerrorを返さない",
			alg:         jwt.SigningMethodHS256,
			claims:      validClaim,
			expected:    nil,
			encodeKey:   "the-secret-key",
			decodeKey:   "the-secret-key",
		},
		{
			description: "algがHS256じゃないとerror",
			alg:         jwt.SigningMethodHS512,
			claims:      validClaim,
			expected:    fmt.Errorf("unexpected signing method: HS512"),
			encodeKey:   "the-secret-key",
			decodeKey:   "the-secret-key",
		},
		{
			description: "expが過ぎるとerror",
			alg:         jwt.SigningMethodHS256,
			claims:      merge(validClaim, jwt.MapClaims{"exp": time.Unix(1, 0).Unix()}),
			expected:    fmt.Errorf("token is expired"),
			encodeKey:   "the-secret-key",
			decodeKey:   "the-secret-key",
		},
		{
			description: "HMACの鍵が違うとエラー",
			alg:         jwt.SigningMethodHS256,
			expected:    fmt.Errorf("signature is invalid"),
			encodeKey:   "the-secret-key",
			decodeKey:   "is-not-same-key",
		},
	}

	for _, testcase := range testcases {
		token := jwt.NewWithClaims(testcase.alg, testcase.claims)
		tokenString, _ := token.SignedString([]byte(testcase.encodeKey))
		os.Setenv("JWT_SECRET_KEY", base64.StdEncoding.EncodeToString([]byte(testcase.decodeKey)))

		_, actual := authJWT(tokenString)

		if actual != nil || testcase.expected != nil {
			if actual == nil {
				actual = errors.New("nil guard for actual")
			}
			if testcase.expected == nil {
				testcase.expected = errors.New("nil guard for testcase.expected")
			}
			if !strings.Contains(actual.Error(), testcase.expected.Error()) {
				t.Fatalf("%s: expected result is %v, but got %v", testcase.description, testcase.expected, actual)
			}
		}

		os.Unsetenv("JWT_SECRET_KEY")
	}
}
