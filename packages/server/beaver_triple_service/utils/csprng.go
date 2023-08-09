package utils

import (
	"bytes"
	"crypto/rand"
	"encoding/binary"
	"errors"
	"fmt"
	"math/big"
)

// [0, 2^{bitLength}) の乱数を amount 個生成
func GetRandBigInts(bitLength uint32, amount uint32) ([]big.Int, error) {
	var byteLength uint32 = (bitLength + 8 - 1) / 8

	bSlice := make([]byte, byteLength * amount)
	_, err := rand.Read(bSlice)
	if err != nil {
		return nil, err
	}

	randSlice := make([]big.Int, amount)

	for i := uint32(0); i < amount; i++ {
		var n big.Int
		buf := bSlice[i*byteLength : (i+1)*byteLength]
		n.SetBytes(buf)
		randSlice[i] = n
	}

	return randSlice, nil
}

func mod(x, y int64) int64 {
	bx, by := big.NewInt(x), big.NewInt(y)
	return new(big.Int).Mod(bx, by).Int64()
}

func GetRandInt64Slice(sliceSize uint64, randMin int64, randMax int64) ([]int64, error) {
	if randMax < randMin {
		return nil, errors.New("randMinがrandMaxより大きい")
	}
	// sliceSize * int64 は sliceSize * (64/8) [bytes]
	bSlice := make([]byte, sliceSize*8)
	_, err := rand.Read(bSlice)
	if err != nil {
		return nil, err
	}

	randSlice := make([]int64, sliceSize)
	for i := uint64(0); i < sliceSize; i++ {
		var n int64
		buf := bytes.NewReader(bSlice[i*8 : (i+1)*8])
		err := binary.Read(buf, binary.LittleEndian, &n)
		if err != nil {
			return nil, err
		}

		tmp := mod(n, randMax-randMin+1)
		rand := tmp + randMin
		if (rand < randMin) || (rand > randMax) {
			return nil, fmt.Errorf("範囲外エラー {n: %d, randMin: %d, randMax: %d, rand: %d, tmp: %d}", n, randMin, randMax, rand, tmp)
		}
		randSlice[i] = rand
	}

	return randSlice, nil
}
