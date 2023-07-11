package e2bserver

import (
	"fmt"
	"os"
	"strings"
	"testing"

	utils "github.com/acompany-develop/QuickMPC/packages/server/beaver_triple_service/utils"
	pb "github.com/acompany-develop/QuickMPC/proto/engine_to_bts"
	"google.golang.org/grpc/codes"
	"google.golang.org/grpc/status"
)

var defaultServerToken string
var defaultClientToken string

const correctServerToken = "Z+k7Cw=="
const correctClientToken = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJyb29tX3V1aWQiOiIxMmRmYWQzMS00ODJiLTQxMjUtYmQ0NS0wOThhMWY3ZTc5ZDMiLCJwYXJ0eV9pZCI6MywicGFydHlfaW5mbyI6W3siaWQiOjEsImFkZHJlc3MiOiIxMC4wLjEuMjAifSx7ImlkIjoyLCJhZGRyZXNzIjoiMTAuMC4yLjIwIn0seyJpZCI6MywiYWRkcmVzcyI6IjEwLjAuMy4yMCJ9XSwic3ViIjoiZTM2ZjQyMGItODE3NS00MWUxLTk2NzQtOGQ5NjQ2ZWExZjMxIiwiZXhwIjo5MjIzMzcxOTc0NzE5MTc4NzUyfQ.8ZZo99laWHdC58RYxEegfUGGKAfeg-8t1uGJR-ZDgig"
const exp0ClientToken = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJyb29tX3V1aWQiOiJhZTAyZTU4Ny0zMGY4LTQyMTgtYTg1Yy00M2I4NjQ2ZWJhMTUiLCJwYXJ0eV9pZCI6MywicGFydHlfaW5mbyI6W3siaWQiOjEsImFkZHJlc3MiOiIxMC4wLjEuMjAifSx7ImlkIjoyLCJhZGRyZXNzIjoiMTAuMC4yLjIwIn0seyJpZCI6MywiYWRkcmVzcyI6IjEwLjAuMy4yMCJ9XSwic3ViIjoiNjU5Yjc0ZTQtMDVhNS00MmU3LThlOTgtMzIwNDU0ZTk0MzYxIiwiZXhwIjowfQ.NJSh9o2NFR3AOwyDt9FRPlFgxKb6Rq4G8YYk1RYi-Zk"
const noneIPClientToken = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJyb29tX3V1aWQiOiJmZTZiODA2MS1lMDkwLTQ1NDctYWQ3OC1lNTcwMzE0ZWE2ZTciLCJwYXJ0eV9pZCI6MywicGFydHlfaW5mbyI6W3siaWQiOjEsImFkZHJlc3MiOiIxMC4wLjEuMjEifSx7ImlkIjoyLCJhZGRyZXNzIjoiMTAuMC4yLjIxIn0seyJpZCI6MywiYWRkcmVzcyI6IjEwLjAuMy4yMSJ9XSwic3ViIjoiYTYyMDIzNTktMjYxOC00Y2Q2LWFjZTQtNzczNDk3MmNmM2VhIiwiZXhwIjo5MjIzMzcxOTc0NzE5MTc4NzUyfQ.VI1R9G1vALlbTDTwzgDKyi3PYOdHyXEPtQxbrgayIf4"
const notHS256ClientToken = "eyJhbGciOiJIUzUxMiIsInR5cCI6IkpXVCJ9.eyJyb29tX3V1aWQiOiIzNTIyOTY3YS0yYTk2LTQ3NzUtOGNhYi1jZGFkMTZlMDA2YzIiLCJwYXJ0eV9pZCI6MywicGFydHlfaW5mbyI6W3siaWQiOjEsImFkZHJlc3MiOiIxMC4wLjEuMjAifSx7ImlkIjoyLCJhZGRyZXNzIjoiMTAuMC4yLjIwIn0seyJpZCI6MywiYWRkcmVzcyI6IjEwLjAuMy4yMCJ9XSwic3ViIjoiMzUyZWZmYTktOGMzZS00ZmZmLTg5ZDQtODkzZDk4YTE2ZmJiIiwiZXhwIjo5MjIzMzcxOTc0NzE5MTc4NzUyfQ.UGMtFKvZvYNNOpDHSKcID2s7jEK4K2zxe_c0jC4zx6WuTlFf-rC5OwGFpRLqNW5ndhNxoj0t4s1erWyXOR5-hA"
const partyIdUnderClientToken = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJyb29tX3V1aWQiOiJjYWIyYTk5ZC0wZDIzLTRiMGItYjc4Ni1jZGU4YzZiNDg5OWEiLCJwYXJ0eV9pZCI6MCwicGFydHlfaW5mbyI6W3siaWQiOjEsImFkZHJlc3MiOiIxMC4wLjEuMjAifSx7ImlkIjoyLCJhZGRyZXNzIjoiMTAuMC4yLjIwIn0seyJpZCI6MywiYWRkcmVzcyI6IjEwLjAuMy4yMCJ9XSwic3ViIjoiN2UyYzRiYjktYWFkMy00ZDZjLTk4MWUtOTYzYWE5ZWRkMmM5IiwiZXhwIjo5MjIzMzcxOTc0NzE5MTc4NzUyfQ.FZXWxSSMX_zrp0aU7ZPHdn6dYAn2Kytl42Do_8C66vs"
const partyIdOverClientToken = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJyb29tX3V1aWQiOiIwZmZiMmQxOS1hY2M0LTRmZjItOGYwMi00ZDZjYjNmNzEwZTAiLCJwYXJ0eV9pZCI6MTAwMDAwLCJwYXJ0eV9pbmZvIjpbeyJpZCI6MSwiYWRkcmVzcyI6IjEwLjAuMS4yMCJ9LHsiaWQiOjIsImFkZHJlc3MiOiIxMC4wLjIuMjAifSx7ImlkIjozLCJhZGRyZXNzIjoiMTAuMC4zLjIwIn1dLCJzdWIiOiI3ZGU5YzFmZi0wMmM0LTRhYTMtYTg1OC03MWEyNzhiZTU4ZGUiLCJleHAiOjkyMjMzNzE5NzQ3MTkxNzg3NTJ9.NUlyABnsaYCEa1aH8JyLna8etFdY4eE2MRGzoW-X6u8"

func init() {
	defaultClientToken = os.Getenv("BTS_TOKEN")
	defaultServerToken = os.Getenv("JWT_BASE64_SECRET_KEY")
	fmt.Println("client: ", defaultClientToken, "\nserver: ", defaultServerToken)
}

func TestSuccessToken(t *testing.T) {
	testcases := map[string]struct {
		serverToken string
		clientToken string
	}{
		"correct_token_1": {defaultServerToken, defaultClientToken},
		"correct_token_2": {correctServerToken, correctClientToken},
	}

	for name, tt := range testcases {
		tt := tt
		t.Run(name, func(t *testing.T) {
			// サーバのtokenを書き換え(できればDIでやりたい)
			serverToken := os.Getenv("JWT_BASE64_SECRET_KEY")
			os.Setenv("JWT_BASE64_SECRET_KEY", tt.serverToken)
			defer os.Setenv("JWT_BASE64_SECRET_KEY", serverToken)

			// サーバを立てる
			s := &utils.TestServer{}
			pb.RegisterEngineToBtsServer(s.GetServer(), &server{})
			s.Serve()

			// requestを送信する
			conn := s.GetConn()
			defer conn.Close()
			client := pb.NewEngineToBtsClient(conn)
			ctx, errCtx := getContext(withToken(tt.clientToken))
			if errCtx != nil {
				t.Fatal(errCtx)
			}
			_, err := client.GetTriples(ctx, &pb.GetRequest{JobId: 1, Amount: 1, TripleType: pb.Type_TYPE_FIXEDPOINT, RequestId: -1})

			// 正しく通信できたか確認
			if err != nil {
				t.Fatalf("request must be success, but reject `%v`", err)
			}
		})
	}
}

func TestFailedIllegalToken(t *testing.T) {
	testcases := map[string]struct {
		serverToken string
		clientToken string
		expected    error
	}{
		"not_jwt_client": {defaultServerToken, "bokuha_jwt_janaiyo",
			status.Error(codes.Unauthenticated, "failed to parse token: token contains an invalid number of segments")},
		"not_jwt_server": {"bokuha_jwt_janaiyo", defaultClientToken,
			status.Error(codes.Unknown, "illegal base64 data")},
		"not_pair_1": {defaultServerToken, correctServerToken,
			status.Error(codes.Unauthenticated, "failed to parse token: token contains an invalid number of segments")},
		"not_pair_2": {correctServerToken, defaultClientToken,
			status.Error(codes.Unauthenticated, "failed to parse token: signature is invalid")},
		"expired": {defaultServerToken, exp0ClientToken,
			status.Error(codes.Unauthenticated, "failed to parse token: token is expired by")},
		"not_HS256": {defaultServerToken, notHS256ClientToken,
			status.Error(codes.Unauthenticated, "failed to parse token: unexpected signing method: HS512")},
		"party_id_under": {defaultServerToken, partyIdUnderClientToken,
			status.Error(codes.Internal, "party_id out of range")},
		"party_id_over": {defaultServerToken, partyIdOverClientToken,
			status.Error(codes.Internal, "party_id out of range")},
		// NOTE: 現状IPのチェックはどこでも行っていない
		// "different_ip": {defaultServerToken, noneIPClientToken,
		// 	status.Error(codes.Unknown, "")},
	}

	for name, tt := range testcases {
		tt := tt
		t.Run(name, func(t *testing.T) {
			// サーバのtokenを書き換え(できればDIでやりたい)
			os.Setenv("JWT_BASE64_SECRET_KEY", tt.serverToken)
			defer os.Setenv("JWT_BASE64_SECRET_KEY", defaultServerToken)

			// サーバを立てる
			s := &utils.TestServer{}
			pb.RegisterEngineToBtsServer(s.GetServer(), &server{})
			s.Serve()

			// requestを送信する
			conn := s.GetConn()
			defer conn.Close()
			client := pb.NewEngineToBtsClient(conn)
			ctx, errCtx := getContext(withToken(tt.clientToken))
			if errCtx != nil {
				t.Fatal(errCtx)
			}
			_, err := client.GetTriples(ctx, &pb.GetRequest{JobId: 1, Amount: 1, TripleType: pb.Type_TYPE_FIXEDPOINT, RequestId: -1})

			// エラーしたかチェック
			// NOTE: error messageがunixtimeに依存するものがあるためcontainsで比較している
			if !strings.Contains(err.Error(), tt.expected.Error()) {
				t.Fatalf("error message must be `%v`, but `%v`", tt.expected, err)
			}
		})
	}
}
