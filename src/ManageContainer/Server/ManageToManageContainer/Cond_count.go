package m2mserver

import (
	"fmt"
	"sync"
	"time"
)

type counter struct {
	count int
	ch    chan bool
	mu    *sync.Mutex
}

var count_map sync.Map
var mu_inc sync.Mutex
var mu_load sync.Mutex

func increment(key string) {
	mu_inc.Lock()
	defer mu_inc.Unlock()

	mp := load(key)
	count_map.Store(key, counter{
		count: mp.count + 1,
		mu:    mp.mu,
		ch:    mp.ch,
	})
	go func() {
		mp.ch <- true
	}()
}

func load(key string) counter {
	mu_load.Lock()
	defer mu_load.Unlock()

	mp_if, ok := count_map.Load(key)
	if !ok {
		mp := counter{
			count: 0,
			mu:    new(sync.Mutex),
			ch:    make(chan bool),
		}
		count_map.Store(key, mp)
		return mp
	}
	mp := mp_if.(counter)
	return mp
}

// fnがtrueとなる間waitする
// keyに対してincrementされる度に判定が起こる
func Wait(key string, waitTime time.Duration, fn func(int) bool) error {

	timeout := time.After(waitTime)
	done := make(chan struct{})
	go func() {
		mp := load(key)
		mp.mu.Lock()
		defer mp.mu.Unlock()

		for fn(load(key).count) {
			b := <-mp.ch
			// timeoutした場合は待機を強制終了する
			if !b {
				break
			}
		}

		count_map.Delete(key)
		done <- struct{}{}
	}()

	select {
	case <-timeout:
		// timeoutを待機処理に通知する
		ma, _ := count_map.Load(key)
		ma.(counter).ch <- false
		// 待機処理の終了を待つ
		<-done
		return fmt.Errorf("Timeout ERROR! Wait %d microseccond, but the condition was not met", waitTime)
	case <-done:
		// 正常に終了
		return nil
	}
}
