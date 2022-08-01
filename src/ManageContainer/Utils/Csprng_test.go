package utils

import (
	"testing"
)

func testCsprng(t *testing.T, sliceSize uint32) {
	t.Helper()
	randSlices, err := CreateRandInt32Slice(sliceSize)
	if err != nil {
		t.Fatal(err)
	}

	if uint32(len(randSlices)) != sliceSize {
		t.Fatal("sliceSizeが合わない")
	}
}

func TestCsprng_1(t *testing.T)       { testCsprng(t, 1) }
func TestCsprng_100(t *testing.T)     { testCsprng(t, 100) }
func TestCsprng_10000(t *testing.T)   { testCsprng(t, 10000) }
func TestCsprng_1000000(t *testing.T) { testCsprng(t, 1000000) }
