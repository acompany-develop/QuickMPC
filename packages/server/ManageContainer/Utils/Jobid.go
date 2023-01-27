package utils

import (
	"github.com/google/uuid"
)

func CreateJobuuid() (string, error) {

	u, err := uuid.NewRandom()
	if err != nil {
		return "", err
	}
	uuidv4 := u.String()

	return uuidv4, nil
}
