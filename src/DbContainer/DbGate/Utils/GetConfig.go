package utils

import (
	"fmt"
	"net/url"
	"os"
)

type IpAddressCollection struct {
	Containers Containers
}

type Containers struct {
	Manage      *url.URL
	Computation *url.URL
	Dbgate      *url.URL
	Sharedb     DbInfo
}

type DbInfo struct {
	Url      *url.URL
	Username string
	Password string
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

	u, err = getEnv("DBGATE")
	if err != nil {
		return IpAddressCollection{}, err
	}
	ipAddrCollection.Containers.Dbgate, err = url.Parse(u)
	if err != nil {
		return IpAddressCollection{}, err
	}

	u, err = getEnv("SHAREDB")
	if err != nil {
		return IpAddressCollection{}, err
	}
	sharedbHost, err := url.Parse(u)
	if err != nil {
		return IpAddressCollection{}, err
	}
	sharedbUsername, err := getEnv("sharedb_CB_USERNAME")
	if err != nil {
		return IpAddressCollection{}, err
	}
	sharedbPassword, err := getEnv("sharedb_CB_PASSWORD")
	if err != nil {
		return IpAddressCollection{}, err
	}

	ipAddrCollection.Containers.Sharedb = DbInfo{sharedbHost, sharedbUsername, sharedbPassword}

	return ipAddrCollection, nil
}
