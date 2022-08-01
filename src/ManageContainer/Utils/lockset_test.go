package utils

import (
	"sync"
	"testing"
	"time"
)

// timeoutを設けてTestするための関数
func timeout_test(t *testing.T, test_func func(), done_func func()) {
	timeout := time.After(3 * time.Second)
	done := make(chan struct{})

	go func() {
		test_func()
		done <- struct{}{}
	}()

	select {
	case <-timeout:
		t.Fatal("Test didn't finish in time")
	case <-done:
		done_func()
	}
}

func TestLockTrue(t *testing.T) {
	/*
	 * lock -> check_num更新 -> 少し待機 -> check_num確認 -> unlock
	 * check_numが変わらないことを確認すればlockが正常に動いている
	 */
	ok := true
	timeout_test(t, func() {
		wg := sync.WaitGroup{}
		set := NewLockSet()
		id := "lock_id"

		check_num := -1
		for i := range [100]int{} {
			index := i
			wg.Add(1)
			go func() {
				set.Lock(id)
				check_num = index
				defer set.Unlock(id)
				defer wg.Done()
				time.Sleep(100 * time.Microsecond)
				if check_num != index {
					ok = false
				}
			}()
		}
		wg.Wait()
	}, func() {
		if !ok {
			t.Fatal("ID lock Failed")
		}
	})
}

func TestLockFalse(t *testing.T) {
	/*
	 * check_num更新 -> 少し待機 -> check_num確認
	 * 上のTestと同じ処理でlockしていなければcheck_numが変わることを確認
	 */
	ok := true
	timeout_test(t, func() {
		wg := sync.WaitGroup{}

		check_num := -1
		for i := range [100]int{} {
			index := i
			wg.Add(1)
			go func() {
				check_num = index
				defer wg.Done()
				time.Sleep(100 * time.Microsecond)
				if check_num != index {
					ok = false
				}
			}()
		}
		wg.Wait()
	}, func() {
		if ok {
			t.Fatal("Success locked. But this test should not be locked")
		}
	})
}

// 異なるIDのlockが干渉しないことを確認
func TestLockMulti(t *testing.T) {
	timeout_test(t, func() {
		set := NewLockSet()
		set.Lock("id1")
		set.Lock("id2")
		set.Lock("id3")
		set.Unlock("id1")
		set.Unlock("id2")
		set.Unlock("id3")
	}, func() {})
}
