package m2mserver

import (
	"sync"
)

type counter struct {
	count int
	ch    chan interface{}
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
			ch:    make(chan interface{}),
		}
		count_map.Store(key, mp)
		return mp
	}
	mp := mp_if.(counter)
	return mp
}

// fnがtrueとなる間waitする
// keyに対してincrementされる度に判定が起こる
func Wait(key string, fn func(int) bool) {
	mp := load(key)
	mp.mu.Lock()
	defer mp.mu.Unlock()

	for fn(load(key).count) {
		<-mp.ch
	}
	count_map.Delete(key)
}
