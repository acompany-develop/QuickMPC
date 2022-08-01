package utils

import (
	"encoding/json"
)

func ConvertJsonStringToStruct(jsonData string, result interface{}) error {
	err := json.Unmarshal([]byte(jsonData), &result)
	if err != nil {
		return err
	}
	return nil
}

func ConvertStructToJsonString(data interface{}) (string, error) {
	b, err := json.Marshal(data)
	if err != nil {
		return "", err
	}
	return string(b), nil
}

func ConvertStructToStruct(data interface{}, result interface{}) error {
	// ※structのfieldにjsonという名前のタグを設定した場合、その値がMapのjsonとなる
	b, err := json.Marshal(data)
	if err != nil {
		return err
	}
	err = json.Unmarshal(b, &result)
	if err != nil {
		return err
	}

	return nil
}
