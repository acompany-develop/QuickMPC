package utils

import (
	"sync"
)

type lockSet struct {
	set sync.Map
}

// mapからchannelを取り出す，ない場合は新しく登録する
func (s *lockSet) get_chan(k string) chan struct{} {
	val, loaded := s.set.LoadOrStore(k, make(chan struct{}))
	ch := val.(chan struct{})
	if !loaded {
		// keyが最初に登録された時はUnlockしておく
		go func() { ch <- struct{}{} }()
	}
	return val.(chan struct{})
}

func NewLockSet() *lockSet {
	return &lockSet{set: sync.Map{}}
}

// Lock(channelの受信待機)
func (s *lockSet) Lock(k string) {
	ch := s.get_chan(k)
	<-ch
}

// UnLock(channelへ送信)
func (s *lockSet) Unlock(k string) {
	ch := s.get_chan(k)
	go func() { ch <- struct{}{} }()
}
