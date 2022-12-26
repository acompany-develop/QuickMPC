package m2mserver

import (
	"sync"
	"testing"
	"time"
)

func generateValue() counter {
	c := counter{
		count: 0,
		mu:    new(sync.Mutex),
		ch:    make(chan bool),
	}
	return c
}

// incrementでcountが増えるかTest
func TestIncrement(t *testing.T) {
	// データ登録
	key := "TestIncrementKey"
	count_map.Store(key, generateValue())

	increment(key)

	// countが増えているか
	val, _ := count_map.Load(key)
	cnt := val.(counter).count
	if cnt != 1 {
		t.Fatalf("cnt must be 1 after increment, but cnt is %d", cnt)
	}

	// データ削除
	count_map.Delete(key)
}

// waitが正常に終了するかTest
func TestWait(t *testing.T) {
	key := "TestWaitKey"

	// 1秒後にincrementで発火する
	go func() {
		time.Sleep(time.Second)
		increment(key)
	}()

	// incrementの発火を待つ
	err := Wait(key, 2*time.Second, func(cnt int) bool {
		return cnt < 1
	})

	if err != nil {
		t.Fatal(err)
	}
}

// WaitがtimeoutするまでWaitしてエラーを返すかテスト
func TestWaitForever(t *testing.T) {
	key := "TestWaitForeverKey"

	// timeoutが来るまでずっとwaitする
	err := Wait(key, time.Second, func(cnt int) bool {
		return true
	})

	if err == nil {
		t.Fatal("Wait must return timeout error, but return nil")
	}
}

// waitが失敗した時にデータが削除されているか
func TestWaitFailedDeleteData(t *testing.T) {
	// データ登録
	key := "TestWaitFailedDeleteDataKey"
	count_map.Store(key, generateValue())

	err := Wait(key, time.Second, func(cnt int) bool {
		return cnt < 1
	})

	if err == nil {
		t.Fatal("Wait must return timeout error, but return nil")
	}

	// データが削除されているか
	_, ok := count_map.Load(key)
	if ok {
		t.Fatal("count_map[key] must be deleted, but exist.")
	}
}
