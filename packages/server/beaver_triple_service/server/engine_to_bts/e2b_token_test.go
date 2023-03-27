package e2bserver

import (
	"fmt"
	"os"
	"testing"

	jwt_types "github.com/acompany-develop/QuickMPC/packages/server/beaver_triple_service/jwt"
	utils "github.com/acompany-develop/QuickMPC/packages/server/beaver_triple_service/utils"
	pb "github.com/acompany-develop/QuickMPC/proto/engine_to_bts"
)

var defaultServerToken string
var defaultClientToken string

const correctServerToken = "6Iwspm8ldN3p9zltzc8kfr2O9ibAG0MzBd6jJXZ5g5A="
const correctClientToken = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJyb29tX3V1aWQiOiI0NGRiYzVkOC0xMWMyLTQyYmItOTY2NS0xOTVkNGI4MWY1NTMiLCJwYXJ0eV9pbmZvIjpbeyJpZCI6MSwiYWRkcmVzcyI6IjEwLjAuMS4yMCJ9LHsiaWQiOjIsImFkZHJlc3MiOiIxMC4wLjIuMjAifSx7ImlkIjozLCJhZGRyZXNzIjoiMTAuMC4zLjIwIn1dLCJpc3MiOiJRdWlja01QQyIsInN1YiI6IjhkNzI1N2ExLTIzODAtNGRlYi1iYjQ5LTc1YTljMGJhYjRmOCIsImF1ZCI6WyJodHRwOi8vbG9jYWxob3N0OjUwMDAxIiwiaHR0cDovL2xvY2FsaG9zdDo1MDAwMiIsImh0dHA6Ly9sb2NhbGhvc3Q6NTAwMDMiXSwiZXhwIjo5MjIzMzcxOTc0NzE5MTc4NzUyfQ.-v0u_V1Gmj_MwKzj7wz5S4rlWEc2Q8F5iZESjOc-76s"
const exp0ServerToken = "7gN0Uh+OsZwP2Dad0kkviHhxT9obkf/yR4rr+FYtbm8="
const exp0ClientToken = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJyb29tX3V1aWQiOiIzODI3ZGZlZi1jODc2LTQyMTEtYmYwNy0xMTk2Nzg4NjlkNTUiLCJwYXJ0eV9pbmZvIjpbeyJpZCI6MSwiYWRkcmVzcyI6IjEwLjAuMS4yMCJ9LHsiaWQiOjIsImFkZHJlc3MiOiIxMC4wLjIuMjAifSx7ImlkIjozLCJhZGRyZXNzIjoiMTAuMC4zLjIwIn1dLCJpc3MiOiJRdWlja01QQyIsInN1YiI6IjEwYTUyMjcxLWNkYWItNDAxNy04Mjg3LTJhN2I5MWU2YWQ4NiIsImF1ZCI6WyJodHRwOi8vbG9jYWxob3N0OjUwMDAxIiwiaHR0cDovL2xvY2FsaG9zdDo1MDAwMiIsImh0dHA6Ly9sb2NhbGhvc3Q6NTAwMDMiXSwiZXhwIjowfQ.RYIZ4ElxDPCvSSICEGP4qd_07yVoAqkdKRB1mE-QQr4"
const noneIPServerToken = "3vvrd+py4htHm3g2+DBL+Gsbm8OK5Wx7ydCCe6D51Ic="
const noneIPClientToken = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJyb29tX3V1aWQiOiJiNzZkYTM2Ny1iYmRkLTRiYTQtOTAzZS02NDMxYTY5NTVjYWQiLCJwYXJ0eV9pbmZvIjpbeyJpZCI6MSwiYWRkcmVzcyI6IjEwMC4wLjEuMjAifSx7ImlkIjoyLCJhZGRyZXNzIjoiMTAwLjAuMi4yMCJ9LHsiaWQiOjMsImFkZHJlc3MiOiIxMDAuMC4zLjIwIn1dLCJpc3MiOiJRdWlja01QQyIsInN1YiI6IjUwMWEwNThkLWMyNDQtNGU5NS1hY2Y5LTkxMWI0NGUwNjg2YiIsImF1ZCI6WyJodHRwOi8vbG9jYWxob3N0OjUwMDAxIiwiaHR0cDovL2xvY2FsaG9zdDo1MDAwMiIsImh0dHA6Ly9sb2NhbGhvc3Q6NTAwMDMiXSwiZXhwIjo5MjIzMzcxOTc0NzE5MTc4NzUyfQ.KwoOnojkZpvB6M_OBOW6bjsl-jpapjQXRpIQRqJipcA"
const notHS256ServerToken = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJyb29tX3V1aWQiOiI0Y2JhNDg4ZS02YjQxLTRmODItODAyMS0wMmQ2MTg0ZmY5M2IiLCJwYXJ0eV9pbmZvIjpbeyJpZCI6MSwiYWRkcmVzcyI6IjEwLjAuMS4yMCJ9LHsiaWQiOjIsImFkZHJlc3MiOiIxMC4wLjIuMjAifSx7ImlkIjozLCJhZGRyZXNzIjoiMTAuMC4zLjIwIn1dLCJpc3MiOiJRdWlja01QQyIsInN1YiI6ImU0ZDc5OGIzLWYwOTYtNGZhOS05MTRjLTAxNzkzMDk2NGY5NCIsImF1ZCI6WyJodHRwOi8vbG9jYWxob3N0OjUwMDAxIiwiaHR0cDovL2xvY2FsaG9zdDo1MDAwMiIsImh0dHA6Ly9sb2NhbGhvc3Q6NTAwMDMiXSwiZXhwIjo5MjIzMzcxOTc0NzE5MTc4NzUyfQ.fvn2Kw_wz3BAhsS6_vTTlHgOzVQqbjPnYdlTzExSUsM"
const notHS256ClientToken = "frkdAtRJogsjDmIHwT4hCYFfJzoNAPUUsIQ8vv8wmJ8="

func init() {
	// モック用GetPartyIdFromIp
	GetPartyIdFromIp = func(claims *jwt_types.Claim, reqIpAddrAndPort string) (uint32, error) {
		if reqIpAddrAndPort == "bufconn" {
			Pic.mux.Lock()
			defer Pic.mux.Unlock()

			if Pic.partyId++; Pic.partyId > uint32(len(claims.PartyInfo)) {
				Pic.partyId = 1
			}
		} else {
			return 0, fmt.Errorf("reqIpAddrAndPortがbufconnではない(%s)", reqIpAddrAndPort)
		}

		return Pic.partyId, nil
	}

	defaultClientToken = os.Getenv("BTS_TOKEN")
	defaultServerToken = os.Getenv("JWT_SECRET_KEY")
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
			// TODO: できればDIでやりたい
			serverToken := os.Getenv("JWT_SECRET_KEY")
			os.Setenv("JWT_SECRET_KEY", tt.serverToken)
			defer os.Setenv("JWT_SECRET_KEY", serverToken)

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
			_, err := client.GetTriples(ctx, &pb.GetTriplesRequest{JobId: 1, Amount: 1, TripleType: pb.Type_TYPE_FIXEDPOINT})

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
	}{
		"形式おかしい":       {defaultServerToken, "bokuha_jwt_janaiyo"},
		"形式おかしい2":      {"bokuha_jwt_janaiyo", defaultClientToken},
		"正常だけど対応してない":  {defaultServerToken, correctServerToken},
		"正常だけど対応してない2": {correctServerToken, defaultClientToken},
		// "IPが違う": {noneIPServerToken, noneIPClientToken},
		"有効期限切れ":    {exp0ServerToken, exp0ClientToken},
		"HS256じゃない": {notHS256ServerToken, notHS256ServerToken},
	}

	for name, tt := range testcases {
		tt := tt
		t.Run(name, func(t *testing.T) {
			// サーバのtokenを書き換え(できればDIでやりたい)
			os.Setenv("JWT_SECRET_KEY", tt.serverToken)
			defer os.Setenv("JWT_SECRET_KEY", defaultServerToken)

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
			_, err := client.GetTriples(ctx, &pb.GetTriplesRequest{JobId: 1, Amount: 1, TripleType: pb.Type_TYPE_FIXEDPOINT})

			// エラーしたかチェック
			if err == nil {
				t.Fatal("request with illegal toke must be error")
			}
			fmt.Println(err)
		})
	}
}
