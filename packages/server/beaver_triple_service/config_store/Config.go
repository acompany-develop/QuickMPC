package config

import (
	"os"

	logger "github.com/acompany-develop/QuickMPC/packages/server/BeaverTripleService/Log"
	utils "github.com/acompany-develop/QuickMPC/packages/server/BeaverTripleService/Utils"
)

type Party struct {
	PartyId   uint32
	IpAddress string
}

type ConfigFormat struct {
	Port             uint32
	PartyNum         uint32
	RequestPartyList []Party
	WithEnvoy          bool
}

var Conf ConfigFormat

func getEnv(key, fallback string) string {
	if value, ok := os.LookupEnv(key); ok {
		return value
	}
	return fallback
}

func init() {
	stage := getEnv("STAGE", "")
	var ConfigJsonFilePath string
	if stage == "src" {
		ConfigJsonFilePath = "/QuickMPC/Config/Config.src.json"
	} else {
		ConfigJsonFilePath = "/QuickMPC/Config/Config.json"
	}
	err := utils.ParseJsonFIle(ConfigJsonFilePath, &Conf)
	if err != nil {
		logger.Fatalf("Config.jsonのParseに失敗: %v", err)
	}
}
