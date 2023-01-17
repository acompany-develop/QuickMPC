package helper

import (
	"context"
	"time"

	. "github.com/acompany-develop/QuickMPC/src/ManageContainer/Log"
	"google.golang.org/grpc"
	"google.golang.org/grpc/codes"
	"google.golang.org/grpc/status"
)

// リトライ回数
const retryNum = 10

// リトライ間隔
const retryWaitTime = 5

type RetryManager struct {
	count int
	Conn  *grpc.ClientConn
}

func (rm *RetryManager) reconnect() {
	// 再接続を試みる
	ctx, cancel := context.WithTimeout(context.Background(), retryWaitTime*time.Second)
	defer cancel()
	rm.Conn.WaitForStateChange(ctx, rm.Conn.GetState())
}

func (rm *RetryManager) canRetry(code codes.Code) bool {

	rm.count++
	if rm.count >= retryNum {
		return false
	}

	// 該当のエラーコードであればリトライする
	if code == codes.DeadlineExceeded ||
		code == codes.Unavailable ||
		code == codes.ResourceExhausted {
		return true
	}
	// retryの余地なし
	return false
}

func (rm *RetryManager) Retry(err error) (bool, error) {
	st, _ := status.FromError(err)

	// 正常に通信できているためリトライ不要
	if st.Code() == codes.OK {
		return false, nil
	}

	AppLogger.Errorf("GRPC Error : Code [%d], Message [%s]", st.Code(), st.Message())
	if rm.canRetry(st.Code()) {
		// リトライ可能と判断して再接続後にリトライ
		rm.reconnect()
		return true, err
	}

	// リトライ不能と判断
	return false, err
}
