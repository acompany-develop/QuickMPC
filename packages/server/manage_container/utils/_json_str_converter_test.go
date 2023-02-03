package utils

import (
	"math"
	"testing"
)

func isDifferentFloatVal(src []float64, dst []float64) bool {
	for i := 0; i < len(src); i++ {
		if math.Abs(src[i]-dst[i]) > 1e-8 {
			return true
		}
	}
	return false
}

func TestConvertJsonStrToMap(t *testing.T) {
	var strResults string = `
		{
			"hoge": "1",
			"huga": "2",
			"piyo": "3"
		}
	`

	results, err := ConvertJsonStrToMap(strResults)
	if err != nil {
		t.Fatal(err)
	}

	if results["hoge"].(string) != "1" {
		t.Fatal("ConvertJsonStrToMap returns invalid first item")
	}

	if results["huga"].(string) != "2" {
		t.Fatal("ConvertJsonStrToMap returns invalid second item")
	}

	if results["piyo"].(string) != "3" {
		t.Fatal("ConvertJsonStrToMap returns invalid third item")
	}
}

func TestConvertJsonStrToInterface(t *testing.T) {
	var strResults string = `
		[
			1.1,2.2,3.3,4.4,5.5,6.6,7.7,8.8,9.9
		]
	`
	groundTruth := []float64{1.1, 2.2, 3.3, 4.4, 5.5, 6.6, 7.7, 8.8, 9.9}

	results, err := ConvertJsonStrToInterface(strResults)
	if err != nil {
		t.Fatal(err)
	}

	arrayResult := results.([]interface{})
	floatResult := []float64{}
	for _, v := range arrayResult {
		floatResult = append(floatResult, v.(float64))
	}

	if isDifferentFloatVal(floatResult, groundTruth) {
		t.Fatal("ConvertJsonStrToInterface returns invalid item")
	}
}

func TestConvertJsonStrToFloatMatrix(t *testing.T) {
	var strResults1 string = `
		[
			["1.1","2.2","3.3"],
			["4.4","5.5","6.6"],
			["7.7","8.8","9.9"]
		]
	`
	var strResults2 string = `
		[
			[1.1,2.2,3.3],
			[4.4,5.5,6.6],
			[7.7,8.8,9.9]
		]
	`

	results1, err := ConvertJsonStrToFloatMatrix(strResults1)
	if err != nil {
		t.Fatal(err)
	}
	results2, err := ConvertJsonStrToFloatMatrix(strResults2)
	if err != nil {
		t.Fatal(err)
	}

	if isDifferentFloatVal(results1[0], []float64{1.1, 2.2, 3.3}) {
		t.Fatal("ConvertJsonStrToFloatMatrix returns invalid first item")
	}
	if isDifferentFloatVal(results1[1], []float64{4.4, 5.5, 6.6}) {
		t.Fatal("ConvertJsonStrToFloatMatrix returns invalid second item")
	}
	if isDifferentFloatVal(results1[2], []float64{7.7, 8.8, 9.9}) {
		t.Fatal("ConvertJsonStrToFloatMatrix returns invalid third item")
	}

	if isDifferentFloatVal(results2[0], []float64{1.1, 2.2, 3.3}) {
		t.Fatal("ConvertJsonStrToFloatMatrix returns invalid first item")
	}
	if isDifferentFloatVal(results2[1], []float64{4.4, 5.5, 6.6}) {
		t.Fatal("ConvertJsonStrToFloatMatrix returns invalid second item")
	}
	if isDifferentFloatVal(results2[2], []float64{7.7, 8.8, 9.9}) {
		t.Fatal("ConvertJsonStrToFloatMatrix returns invalid third item")
	}
}

func TestConvertToJsonstr(t *testing.T) {
	var groundTruth string = "[[1.1,2.2,3.3],[4.4,5.5,6.6],[7.7,8.8,9.9]]"

	data := [][]float64{{1.1, 2.2, 3.3}, {4.4, 5.5, 6.6}, {7.7, 8.8, 9.9}}

	results, err := ConvertToJsonstr(data)
	if err != nil {
		t.Fatal(err)
	}

	if results != groundTruth {
		t.Fatal("ConvertJsonStrToList returns invalid first item")
	}

}
