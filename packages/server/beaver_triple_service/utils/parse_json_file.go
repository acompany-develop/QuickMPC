package utils

import (
	"encoding/json"
	"io/ioutil"
)

func ParseJsonFIle(filePath string, strcutInterface interface{}) error {
	data, err := ioutil.ReadFile(filePath)
	if err != nil {
		return err
	}

	err = json.Unmarshal(data, &strcutInterface)
	if err != nil {
		return err
	}

	return nil
}
