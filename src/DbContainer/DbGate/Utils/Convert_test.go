package utils

import (
	"reflect"
	"strings"
	"testing"
)

var strResults string = `
	[{
		"hoge": "1",
		"huga": 2,
		"piyo": 3,
		"payo": 4,
		"puyo": 5.1,
		"poyo": "string"
	},{
		"hoge": "1",
		"huga": 2,
		"piyo": 3,
		"payo": 4,
		"puyo": 5.1,
		"poyo": "string"
	}]
`
var trimStrResults string = strings.ReplaceAll(strings.ReplaceAll(strings.ReplaceAll(strResults, "\t", ""), "\n", ""), " ", "")

type ResultStruct struct {
	Hoge string  `required:"true" json:"hoge"`
	Huga int     `required:"true" json:"huga"`
	Piyo uint64  `required:"true" json:"piyo"`
	Payo float64 `required:"true" json:"payo"`
	Puyo float64 `required:"true" json:"puyo"`
	Poyo string  `required:"true" json:"poyo"`
}

func TestConvertJsonStringToStruct(t *testing.T) {
	var results1 []ResultStruct
	err := ConvertJsonStringToStruct(strResults, &results1)
	if err != nil {
		t.Fatal(err)
	}

	for _, res := range results1 {
		if res.Hoge != "1" {
			t.Fatal("ConvertJsonStringToStruct returns invalid first item")
		}

		if res.Huga != int(2) {
			t.Fatal("ConvertJsonStringToStruct returns invalid second item")
		}

		if res.Piyo != uint64(3) {
			t.Fatal("ConvertJsonStringToStruct returns invalid third item")
		}

		if res.Payo != float64(4) {
			t.Fatal("ConvertJsonStringToStruct returns invalid third item")
		}

		if res.Puyo != float64(5.1) {
			t.Fatal("ConvertJsonStringToStruct returns invalid third item")
		}

		if res.Poyo != "string" {
			t.Fatal("ConvertJsonStringToStruct returns invalid third item")
		}
	}

	results2 := []map[string]interface{}{}
	err = ConvertJsonStringToStruct(strResults, &results2)
	if err != nil {
		t.Fatal(err)
	}
	for _, res := range results2 {
		if res["hoge"] != "1" {
			t.Fatal("ConvertJsonStringToStruct returns invalid first item")
		}

		if res["huga"] != float64(2.0) {
			t.Fatal("ConvertJsonStringToStruct returns invalid second item")
		}

		if res["piyo"] != float64(3.0) {
			t.Fatal("ConvertJsonStringToStruct returns invalid third item")
		}

		if res["payo"] != float64(4.0) {
			t.Fatal("ConvertJsonStringToStruct returns invalid third item")
		}

		if res["puyo"] != float64(5.1) {
			t.Fatal("ConvertJsonStringToStruct returns invalid third item")
		}

		if res["poyo"] != "string" {
			t.Fatal("ConvertJsonStringToStruct returns invalid third item")
		}
	}
}

func TestConvertStructToJsonString(t *testing.T) {
	results1 := []ResultStruct{
		{
			Hoge: "1",
			Huga: int(2),
			Piyo: uint64(3),
			Payo: float64(4),
			Puyo: float64(5.1),
			Poyo: "string",
		},
		{
			Hoge: "1",
			Huga: int(2),
			Piyo: uint64(3),
			Payo: float64(4),
			Puyo: float64(5.1),
			Poyo: "string",
		},
	}

	resultString, err := ConvertStructToJsonString(results1)
	if err != nil {
		t.Fatal(err)
	}

	if resultString != trimStrResults {
		t.Fatal("ConvertStructToJsonString returns invalid third item")
	}
}

func TestConvertStructToStruct(t *testing.T) {
	jsonMaps := []map[string]interface{}{}
	err := ConvertJsonStringToStruct(strResults, &jsonMaps)
	if err != nil {
		t.Fatal(err)
	}
	results := []ResultStruct{}
	err = ConvertStructToStruct(jsonMaps, &results)
	if err != nil {
		t.Fatal(err)
	}
	jsonMaps2 := []map[string]interface{}{}
	err = ConvertStructToStruct(results, &jsonMaps2)
	if err != nil {
		t.Fatal(err)
	}

	if !reflect.DeepEqual(jsonMaps2, jsonMaps) {
		t.Fatal("ConvertStructToStruct returns invalid")
	}

}

func TestConvertStructToJson(t *testing.T) {
	type Person struct {
		Name string `json:"name"` // jsonを指定するとこれがJsonのkeyになる
		Age  int
	}

	p := Person{"hoge", 22}

	results := make(map[string]interface{})
	_ = ConvertStructToStruct(p, &results)
	// t.Log(results["name"], reflect.TypeOf(results["name"]))
	// t.Log(results["Age"], reflect.TypeOf(results["Age"]))

	if results["name"] != "hoge" {
		t.Fatal("ConvertStructToMap returns invalid first item")
	}

	if results["Age"] != float64(22) {
		// jsonは数値をfloat64として扱っている
		t.Fatal("ConvertStructToMap returns invalid second item")
	}
}
