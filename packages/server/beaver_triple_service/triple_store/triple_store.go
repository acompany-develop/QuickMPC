package triplestore

import (
	"sync"

	logger "github.com/acompany-develop/QuickMPC/packages/server/beaver_triple_service/log"
	pb "github.com/acompany-develop/QuickMPC/proto/engine_to_bts"
)

// TripleGeneratorでpbをimportしなくていいようにaliasを貼る
type Triple = pb.Triple

type SafeTripleStore struct {
	Triples map[uint32](map[uint32]([]*Triple))
	Mux     sync.Mutex
}

var instance *SafeTripleStore
var once sync.Once

func GetInstance() *SafeTripleStore {
	once.Do(func() {
		logger.Info("new instance")
		instance = &SafeTripleStore{Triples: make(map[uint32](map[uint32]([]*Triple)))}
	})
	return instance
}
