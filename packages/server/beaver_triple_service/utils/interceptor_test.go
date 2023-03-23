package utils

import (
	"errors"
	"fmt"
	"os"
	"strings"
	"testing"
	"time"

	"github.com/golang-jwt/jwt/v4"
)

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

	validPartyInfo := []map[string]interface{}{}
	validPartyInfo = append(validPartyInfo, map[string]interface{}{
		"id": 1,
		"address":  "10.0.1.20",
	})
	validPartyInfo = append(validPartyInfo, map[string]interface{}{
		"id": 2,
		"address":  "10.0.2.20",
	})
	validPartyInfo = append(validPartyInfo, map[string]interface{}{
		"id": 3,
		"address":  "10.0.3.20",
	})
	validClaim := jwt.MapClaims{
		"party_id": 1,
		"party_info": validPartyInfo,
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
			description: "HMACの鍵が違うとerror",
			alg:         jwt.SigningMethodHS256,
			expected:    fmt.Errorf("signature is invalid"),
			encodeKey:   "the-secret-key",
			decodeKey:   "is-not-same-key",
		},
		{
			description: "party_idが異常な値だとエラー(1未満)",
			alg:         jwt.SigningMethodHS256,
			claims: merge(validClaim, jwt.MapClaims{"party_id": 0}),
			expected:    fmt.Errorf("party_id out of range"),
			encodeKey:   "the-secret-key",
			decodeKey:   "the-secret-key",
		},
		{
			description: "party_idが異常な値だとエラー(len(PartyInfo)超過)",
			alg:         jwt.SigningMethodHS256,
			claims: merge(validClaim, jwt.MapClaims{"party_id": uint32(len(validPartyInfo)+1)}),
			expected:    fmt.Errorf("party_id out of range"),
			encodeKey:   "the-secret-key",
			decodeKey:   "the-secret-key",
		},
	}

	for _, testcase := range testcases {
		token := jwt.NewWithClaims(testcase.alg, testcase.claims)
		tokenString, _ := token.SignedString([]byte(testcase.encodeKey))
		os.Setenv("JWT_SECRET_KEY", testcase.decodeKey)

		_, actual := AuthJWT(tokenString)

		if actual != nil || testcase.expected != nil {
			if actual == nil {
				actual = errors.New("nil guard for actual")
			}
			if testcase.expected == nil {
				testcase.expected = errors.New("nil guard for testcase.expected")
			}
			if !strings.Contains(actual.Error(), testcase.expected.Error()) {
				t.Fatalf("%s: expected result is \"%v\", but got \"%v\"", testcase.description, testcase.expected, actual)
			}
		}

		os.Unsetenv("JWT_SECRET_KEY")
	}
}
