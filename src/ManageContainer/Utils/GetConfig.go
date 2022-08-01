package utils

import (
	"fmt"
	"net/url"
	"os"
	"strconv"
	"strings"
)

type IpAddressCollection struct {
	PartyID       int
	NParties      int
	Containers    Containers
	AllowedOrigin []*url.URL
}

type Containers struct {
	Manage      *url.URL
	Computation *url.URL
	Dbg         *url.URL
	PartyList   []Party
}

type Party struct {
	PartyID   int
	IpAddress *url.URL
}

func getEnv(key string) (string, error) {
	if value, ok := os.LookupEnv(key); ok {
		return value, nil
	}
	return "", fmt.Errorf("%s is not defined", key)
}

// TODO: errにメッセージをつけてstacktraceを返すようにする
// "github.com/pkg/errors"のerrors.WithMessageを使用推奨
func GetConfig() (IpAddressCollection, error) {
	var ipAddrCollection IpAddressCollection
	var err error
	var partyID string
	var nParties string

	partyID, err = getEnv("PARTY_ID")
	if err != nil {
		return IpAddressCollection{}, err
	}
	ipAddrCollection.PartyID, _ = strconv.Atoi(partyID)

	nParties, err = getEnv("N_PARTIES")
	if err != nil {
		return IpAddressCollection{}, err
	}
	ipAddrCollection.NParties, _ = strconv.Atoi(nParties)

	u, err := getEnv("MANAGE")
	if err != nil {
		return IpAddressCollection{}, err
	}
	ipAddrCollection.Containers.Manage, err = url.Parse(u)
	if err != nil {
		return IpAddressCollection{}, err
	}

	u, err = getEnv("COMPUTATION")
	if err != nil {
		return IpAddressCollection{}, err
	}
	ipAddrCollection.Containers.Computation, err = url.Parse(u)
	if err != nil {
		return IpAddressCollection{}, err
	}

	u, err = getEnv("DBG")
	if err != nil {
		return IpAddressCollection{}, err
	}
	ipAddrCollection.Containers.Dbg, err = url.Parse(u)
	if err != nil {
		return IpAddressCollection{}, err
	}

	for id := 1; id <= ipAddrCollection.NParties; id++ {
		var party Party
		party.PartyID = id
		u, err = getEnv("PARTY_LIST" + strconv.Itoa(id))
		if err != nil {
			return IpAddressCollection{}, err
		}
		party.IpAddress, err = url.Parse(u)
		if err != nil {
			return IpAddressCollection{}, err
		}
		ipAddrCollection.Containers.PartyList = append(ipAddrCollection.Containers.PartyList, party)
	}

	us, err := getEnv("ALLOWEDORIGIN")
	if err != nil {
		return IpAddressCollection{}, err
	}
	uSlice := strings.Split(us, ",")

	var allowedOrigin *url.URL
	for _, u := range uSlice {
		allowedOrigin, err = url.Parse(u)
		if err != nil {
			return IpAddressCollection{}, err
		}
		ipAddrCollection.AllowedOrigin = append(ipAddrCollection.AllowedOrigin, allowedOrigin)
	}

	return ipAddrCollection, nil
}
