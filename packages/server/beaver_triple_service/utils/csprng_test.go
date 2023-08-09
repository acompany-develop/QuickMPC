package utils_test

import (
	"testing"

	utils "github.com/acompany-develop/QuickMPC/packages/server/beaver_triple_service/utils"
)

func TestGetRandBigInts(t *testing.T){
	t.Helper()
	rands, err := utils.GetRandBigInts(90, 100000)
	if err != nil{
		t.Fatal(err)
	}

	x := big.NewInt(0)

	for _, rand := range rands {
		sgn := rand.Sign()
		if sgn < 0 {
			err := errors.New("negative rand")
			t.Fatal(err)
		}
		x.Or(x, rand)
	}

	expected := big.NewInt(1)
	expected.Lsh(expected, 96)
	expected.Sub(expected, 1)

	diff := x.Cmp(expected)
	if diff != 0 {
		err := errors.New("missing bit")
		t.Fatal(err)
	}
}

var randMax = int64(10000000)
var randMin = int64(-10000000)

func testGetRandInt64Slice(t *testing.T, sliceSize uint64, randMin int64, randMax int64) {
	t.Helper()
	randSlices, err := utils.GetRandInt64Slice(sliceSize, randMin, randMax)
	if err != nil {
		t.Fatal(err)
	}

	if uint64(len(randSlices)) != sliceSize {
		t.Fatal("sliceSizeが合わない")
	}

	for _, r := range randSlices {
		if (r < randMin) || (r > randMax) {
			t.Log(r)
			t.Fatal("乱数の範囲エラー")
		}
	}
}

func TestGetRandInt64Slice_1(t *testing.T)       { testGetRandInt64Slice(t, 1, randMin, randMax) }
func TestGetRandInt64Slice_100(t *testing.T)     { testGetRandInt64Slice(t, 100, randMin, randMax) }
func TestGetRandInt64Slice_10000(t *testing.T)   { testGetRandInt64Slice(t, 10000, randMin, randMax) }
func TestGetRandInt64Slice_1000000(t *testing.T) { testGetRandInt64Slice(t, 1000000, randMin, randMax) }

func TestGetRandInt64Slice_CornerCase_1(t *testing.T) {
	// sliceSize = 1, randMin == randMaxのケース
	sliceSize := uint64(1)
	randMin := int64(1)
	randMax := int64(1)

	t.Helper()
	randSlices, err := utils.GetRandInt64Slice(sliceSize, randMin, randMax)
	if err != nil {
		t.Fatal(err)
	}

	if uint64(len(randSlices)) != sliceSize {
		t.Fatal("sliceSizeが合わない")
	}

	for _, r := range randSlices {
		if (r < randMin) || (r > randMax) {
			t.Log(r)
			t.Fatal("乱数の範囲エラー")
		}
	}
}

func TestGetRandInt64Slice_CornerCase_2(t *testing.T) {
	// 閉区間で生成できているかの確認
	sliceSize := uint64(1000)
	randMin := int64(-1)
	randMax := int64(1)

	t.Helper()
	randSlices, err := utils.GetRandInt64Slice(sliceSize, randMin, randMax)
	if err != nil {
		t.Fatal(err)
	}

	if uint64(len(randSlices)) != sliceSize {
		t.Fatal("sliceSizeが合わない")
	}

	randMinIsOk := false
	randMaxIsOk := false
	for i := uint64(0); i < uint64(len(randSlices)); i++ {
		if randSlices[i] == randMin {
			randMinIsOk = true
		}
		if randSlices[i] == randMax {
			randMaxIsOk = true
		}
	}

	if !(randMinIsOk && randMaxIsOk) {
		t.Fatal("乱数の値域が閉区間ではなさそう")
	}

	for _, r := range randSlices {
		if (r < randMin) || (r > randMax) {
			t.Log(r)
			t.Fatal("乱数の値域範囲エラー")
		}
	}
}
