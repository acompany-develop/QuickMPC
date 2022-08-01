package utils

import (
	"strings"
)

type NiqlParamType int

// Enum
const (
	Select NiqlParamType = iota // Select  == 0
	Where                       // Where   == 1
)

func EscapeInjection(in string, nptype NiqlParamType) string {
	out := in

	switch nptype {
		case Select:
			out = strings.Replace(in, "`", "``", -1)
		case Where:
			out = strings.Replace(strings.Replace(in, "'", "''", -1), "\"", "\"\"", -1)
	}

	return out
}
