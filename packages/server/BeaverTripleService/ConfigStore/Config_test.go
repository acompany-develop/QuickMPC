package config_test

import (
	"testing"

	cs "github.com/acompany-develop/QuickMPC/packages/server/BeaverTripleService/ConfigStore"
)

func TestConfigStore(t *testing.T) {
	t.Helper()
	t.Log(cs.Conf)

	if cs.Conf.Port == 0 {
		t.Fatal("Portの読み込み失敗")
	}
	if cs.Conf.PartyNum == 0 {
		t.Fatal("PartyNumの読み込み失敗")
	}
	if len(cs.Conf.RequestPartyList) == 0 {
		t.Fatal("PartyListの読み込み失敗")
	} else {
		if uint32(len(cs.Conf.RequestPartyList)) != cs.Conf.PartyNum {
			t.Fatal("PartyListの要素数が違う")
		} else {
			if cs.Conf.RequestPartyList[0].PartyId == 0 {
				t.Fatal("PartyIdは1以上")
			}
			if len(cs.Conf.RequestPartyList[0].IpAddress) == 0  {
				t.Fatal("IpAddressが空文字")
			}
		}
	}
}
