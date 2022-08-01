package m2mserver

import (
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
	timeout := time.After(2 * time.Second)
	done := make(chan bool)

	go func() {
		time.Sleep(time.Second)
		increment(id)
	}()

	go func() {
		// incrementで発火してdoneする
		Wait(id, func(cnt int) bool {
			return cnt < 1
		})
		done <- true
	}()

	select {
	case <-timeout:
		t.Fatal("")
	case <-done:
	}
}

// waitがwaitし続けるかTest
func TestWaitForever(t *testing.T) {
	id := "id"
	timeout := time.After(time.Second)
	done := make(chan bool)
	go func() {
		// incrementがないためずっとwaitする
		Wait(id, func(cnt int) bool {
			return cnt < 1
		})
		done <- true
	}()

	select {
	case <-done:
		t.Fatal("")
	case <-timeout:
	}
}
