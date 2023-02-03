package utils_test

import (
	"testing"

	utils "github.com/acompany-develop/QuickMPC/packages/server/beaver_triple_service/utils"
)

func TestParseJsonFIle(t *testing.T) {
	conf := map[string]interface{}{}

	// 絶対パスで指定
	err := utils.ParseJsonFIle("/QuickMPC/config/config.json", &conf)
	if err != nil {
		t.Fatal(err)
	}
}
