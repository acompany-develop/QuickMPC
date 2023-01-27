package utils

import (
	"bytes"
	"crypto/rand"
	"encoding/binary"
)

func CreateRandInt32Slice(sliceSize uint32) ([]int32, error) {

	// sliceSize * int32 は sliceSize * (32/8) [bytes]
	bSlice := make([]byte, sliceSize*4)
	_, err := rand.Read(bSlice)
	if err != nil {
		return nil, err
	}

	randSlice := make([]int32, sliceSize)
	for i := uint32(0); i < sliceSize; i++ {
		var rand int32
		buf := bytes.NewReader(bSlice[i*4 : (i+1)*4])
		err := binary.Read(buf, binary.LittleEndian, &rand)
		if err != nil {
			return nil, err
		}

		randSlice[i] = rand
	}

	return randSlice, nil
}
