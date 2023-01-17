package helper

import (
	"testing"

	utils "github.com/acompany-develop/QuickMPC/src/ManageContainer/Utils"
	"google.golang.org/grpc/codes"
	"google.golang.org/grpc/status"
)

var unavailableErr = status.Error(codes.Unavailable, "")

// Test用のempty server
var s *utils.TestServer

func init() {
	s = &utils.TestServer{}
	s.GetServer()
	s.Serve()
}

// 正常の場合リトライがfalseになるか
func TestRetryNotError(t *testing.T) {
	rm := RetryManager{Conn: s.GetConn()}
	b, err := rm.Retry(nil)
	if b || err != nil {
		t.Fatalf("Retry must return `(false, nil)` when argument `err` is nil. but return `(%t, %v)`", b, err)
	}
}

// 異常の場合リトライがtrueになるか
func TestRetryError(t *testing.T) {
	rm := RetryManager{Conn: s.GetConn()}
	b, err := rm.Retry(unavailableErr)
	if !b || err == nil {
		t.Fatalf("Retry must return `(true, err)` when argument `err` has error. but return `(%t, %v)`", b, err)
	}
}

// Grpcの各StatusのErrorでRetryを区別できているか
func TestRetryGrpcError(t *testing.T) {
	testcases := map[string]struct {
		grpcErr  error
		expected bool
	}{
		"OK":                 {status.Error(codes.OK, ""), false},
		"Canceled":           {status.Error(codes.Canceled, ""), false},
		"Unknown":            {status.Error(codes.Unknown, ""), false},
		"InvalidArgument":    {status.Error(codes.InvalidArgument, ""), false},
		"DeadlineExceeded":   {status.Error(codes.DeadlineExceeded, ""), true},
		"NotFound":           {status.Error(codes.NotFound, ""), false},
		"AlreadyExists":      {status.Error(codes.AlreadyExists, ""), false},
		"PermissionDenied":   {status.Error(codes.PermissionDenied, ""), false},
		"ResourceExhausted":  {status.Error(codes.ResourceExhausted, ""), true},
		"FailedPrecondition": {status.Error(codes.FailedPrecondition, ""), false},
		"Aborted":            {status.Error(codes.Aborted, ""), false},
		"OutOfRange":         {status.Error(codes.OutOfRange, ""), false},
		"Unimplemented":      {status.Error(codes.Unimplemented, ""), false},
		"Internal":           {status.Error(codes.Internal, ""), false},
		"Unavailable":        {status.Error(codes.Unavailable, "ERROR"), true},
		"DataLoss":           {status.Error(codes.DataLoss, ""), false},
		"Unauthenticated":    {status.Error(codes.Unauthenticated, ""), false},
	}

	for name, tt := range testcases {
		tt := tt
		t.Run(name, func(t *testing.T) {
			t.Parallel()

			rm := RetryManager{Conn: s.GetConn()}
			b, _ := rm.Retry(tt.grpcErr)
			if b != tt.expected {
				t.Fatalf("Retry must return `%t` when grpc error is `%s`, but return `%t`", tt.expected, name, b)
			}
		})
	}
}

// リトライ回数制限が動作するか
func TestRetryErrorMomentary(t *testing.T) {
	rm := RetryManager{Conn: s.GetConn()}

	// 9回まではリトライする
	for i := 0; i < 9; i++ {
		b, err := rm.Retry(unavailableErr)
		if !b || err == nil {
			t.Fatalf("Retry must return `(true, err)` when argument `err` has error. but return `(%t, %v)`", b, err)
		}
	}

	// 10回目はリトライしない
	b, err := rm.Retry(unavailableErr)
	if b || err == nil {
		t.Fatalf("Retry must return `false, err` when call Retry over 10 times. but return `%t %v`", b, err)
	}
}
