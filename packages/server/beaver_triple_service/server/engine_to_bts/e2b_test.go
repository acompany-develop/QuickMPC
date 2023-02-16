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

	cs "github.com/acompany-develop/QuickMPC/packages/server/beaver_triple_service/config_store"
	ts "github.com/acompany-develop/QuickMPC/packages/server/beaver_triple_service/triple_store"
	utils "github.com/acompany-develop/QuickMPC/packages/server/beaver_triple_service/utils"
	pb "github.com/acompany-develop/QuickMPC/proto/engine_to_bts"
	emptypb "google.golang.org/protobuf/types/known/emptypb"
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

func TestInitTripleStore(t * testing.T) {
	conn := s.GetConn()
	defer conn.Close()
	client := pb.NewEngineToBtsClient(conn)

	_,err := client.InitTripleStore(context.Background(), &emptypb.Empty{})
	if err != nil {
		t.Fatal(err)
	}
}

func TestDeleteJobIdTriple(t * testing.T) {
	conn := s.GetConn()
	defer conn.Close()
	client := pb.NewEngineToBtsClient(conn)

	_,err := client.DeleteJobIdTriple(context.Background(), &pb.DeleteJobIdTripleRequest{JobId: 0})
	if err != nil {
		t.Fatal(err)
	}
}
