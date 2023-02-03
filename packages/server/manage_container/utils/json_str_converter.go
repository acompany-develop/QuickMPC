package utils

import (
	"encoding/json"
	"errors"
	"fmt"
	"strconv"
)

func ConvertJsonStrToMap(data string) (map[string]interface{}, error) {
	var result map[string]interface{}
	err := json.Unmarshal([]byte(data), &result)
	if err != nil {
		return nil, err
	}
	return result, nil
}

func ConvertJsonStrToInterface(data string) (interface{}, error) {
	var result interface{}
	err := json.Unmarshal([]byte(data), &result)
	if err != nil {
		return nil, err
	}
	return result, nil
}

func ConvertJsonStrToFloatMatrix(data string) ([][]float64, error) {
	var resultInterface [][]interface{}
	var resultFloat [][]float64
	err := json.Unmarshal([]byte(data), &resultInterface)
	if err != nil {
		return nil, err
	}

	for _, interfaces := range resultInterface {
		floatArray := make([]float64, len(interfaces))
		for j, i := range interfaces {
			var floatVal float64

			switch v := i.(type) {
			case int:
				floatVal = float64(v)
			case string:
				f, err := strconv.ParseFloat(v, 64)
				if err != nil {
					return nil, err
				}
				floatVal = f
			case float64:
				floatVal = v
			default:
				str := fmt.Sprintf("I don't know about type %T!\n", v)
				return nil, errors.New(str)
			}

			floatArray[j] = floatVal
		}
		resultFloat = append(resultFloat, floatArray)
	}

	return resultFloat, nil
}

func ConvertToJsonstr(data interface{}) (string, error) {
	jsonByte, err := json.Marshal(data)
	if err != nil {
		return "", err
	}
	jsonStr := string(jsonByte)
	return jsonStr, nil
}
