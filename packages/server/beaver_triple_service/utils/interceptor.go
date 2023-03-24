package utils

import (
	"context"
	"fmt"
	"os"
	"time"

	"github.com/golang-jwt/jwt/v4"
	grpc_auth "github.com/grpc-ecosystem/go-grpc-middleware/auth"
	"google.golang.org/grpc"
	"google.golang.org/grpc/codes"
	"google.golang.org/grpc/status"

	jwt_types "github.com/acompany-develop/QuickMPC/packages/server/beaver_triple_service/jwt"
	logger "github.com/acompany-develop/QuickMPC/packages/server/beaver_triple_service/log"
)

func BtsAuthFunc(ctx context.Context) (context.Context, error) {
	tokenString, err := grpc_auth.AuthFromMD(ctx, "bearer")
	if err != nil {
		return nil, status.Errorf(
			codes.Unauthenticated,
			"could not read auth token: %v",
			err,
		)
	}

	claims, err := AuthJWT(tokenString)
	if err != nil {
		return nil, err
	}

	return context.WithValue(ctx, "claims", claims), nil
}

func getSecret() ([]byte, error) {
	var secrets []byte
	if os.Getenv("JWT_SECRET_KEY") != "" {
		secrets = []byte(os.Getenv("JWT_SECRET_KEY"))
	} else {
		secrets = []byte("hoge")
	}

	return secrets, nil
}

func AuthJWT(tokenString string) (*jwt_types.Claim, error) {
	jwtSecret, err := getSecret()
	if err != nil {
		return nil, err
	}

	token, err := jwt.ParseWithClaims(tokenString, &jwt_types.Claim{}, func(token *jwt.Token) (interface{}, error) {
		// alg を確認するのを忘れない
		if signingMethod, ok := token.Method.(*jwt.SigningMethodHMAC); !ok || signingMethod.Alg() != "HS256" {
			return nil, fmt.Errorf("unexpected signing method: %v", token.Header["alg"])
		}
		return jwtSecret, nil
	})

	if err != nil {
		return nil, status.Errorf(codes.Unauthenticated, "failed to parse token: %v", err)
	}

	if !token.Valid {
		return nil, status.Errorf(codes.Unauthenticated, "invalid auth token: %v", err)
	}

	claims, ok := token.Claims.(*jwt_types.Claim)
	if !ok {
		return nil, status.Error(codes.Internal, "failed claims type assertions")
	}

	// expのバリデーション
	if claims, ok := token.Claims.(jwt.MapClaims); ok && token.Valid {
		exp := time.Unix(int64(claims["exp"].(float64)), 0)
		if time.Now().After(exp) {
			return nil, status.Error(codes.Internal, "token is expired")
		}
	}

	// party_idのバリデーション
	if claims.PartyId < 1 || claims.PartyId > uint32(len(claims.PartyInfo)) {
		return nil, status.Error(codes.Internal, "party_id out of range")
	}

	return claims, nil
}

// requestを受け取った際の共通処理
func UnaryInterceptor(ctx context.Context, req interface{}, info *grpc.UnaryServerInfo, handler grpc.UnaryHandler) (interface{}, error) {
	// 定期実行されるhealth checkでlogは必要ないため即時return
	if info.FullMethod == "/grpc.health.v1.Health/Check" {
		return handler(ctx, req)
	}

	logger.Infof("received: %s", info.FullMethod)

	// TODO: read claims, and use these party information
	claims, ok := ctx.Value("claims").(*jwt_types.Claim)
	if ok {
		logger.Infof("claims: %v\n", claims)
	}

	// 処理を実行する
	res, err := handler(ctx, req)

	// エラー時にログとしてrequest，responseを出力する
	if err != nil {
		logger.Errorf("request: {%v}\tresponse: {%v}\n", req, res)
	}

	logger.Infof("send: %s", info.FullMethod)
	return res, err
}
