package m2mserver

import (
	"sync"
	"testing"
	"time"
)

// incrementでcountが増えるかTest
func TestIncrement(t *testing.T) {
	id := "id"
	if load(id).count != 0 {
		t.Fatal()
	}
	increment(id)
	if load(id).count != 1 {
		t.Fatal()
	}
	count_map.Delete(id)
}

// waitが正常に終了するかTest
func TestWait(t *testing.T) {
	id := "id"

	go func() {
		time.Sleep(time.Second)
		increment(id)
	}()

	// incrementで発火する
	err := Wait(id, 2*time.Second, func(cnt int) bool {
		return cnt < 1
	})

	if err != nil {
		t.Fatal("", err)
	}
}

// waitがwaitし続けるかTest
func TestWaitForever(t *testing.T) {
	id := "id"

	// incrementがないためずっとwaitする
	err := Wait(id, time.Second, func(cnt int) bool {
		return cnt < 1
	})

	if err == nil {
		t.Fatal("", err)
	}
}

// waitが失敗した時にデータが削除されているか
func TestWaitFailedDeleteData(t *testing.T) {
	id := "id"
	mp := counter{
		count: 0,
		mu:    new(sync.Mutex),
		ch:    make(chan bool),
	}

	// データ登録
	count_map.Store(id, mp)

	err := Wait(id, time.Second, func(cnt int) bool {
		return cnt < 1
	})
	if err == nil {
		t.Fatal("", err)
	}

	// データが削除されているか
	_, ok := count_map.Load(id)
	if ok {
		t.Fatal("")
	}
}
