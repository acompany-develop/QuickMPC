package randbitstore

import (
	"sync"

	logger "github.com/acompany-develop/QuickMPC/packages/server/beaver_triple_service/log"
)

type SafeRandBitStore struct {
	RandBits map[uint32](map[uint32]([]*int64))
	PreID map[uint32](map[uint32](int64))
	PreAmount map[uint32](map[uint32](uint32))
	Mux     sync.Mutex
}

var instance *SafeRandBitStore
var once sync.Once

func GetInstance() *SafeRandBitStore {
	once.Do(func() {
		logger.Info("new instance")
		instance = &SafeRandBitStore{
			RandBits: make(map[uint32](map[uint32]([]*int64))),
			PreID: make(map[uint32](map[uint32](int64))),
			PreAmount: make(map[uint32](map[uint32](uint32))),
		}
	})
	return instance
}
