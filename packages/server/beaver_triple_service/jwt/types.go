package jwt

import "github.com/golang-jwt/jwt/v4"

type Party struct {
	Id      uint32    `json:"id"`
	Address string `json:"address"`
}

type Claim struct {
	RoomUUID  string  `json:"room_uuid"`
	Port      uint32  `json:"port"`
	PartyNum  uint32  `json:"party_num"`
	PartyInfo []Party `json:"party_info"`
	WithEnvoy bool    `json:"with_envoy"`
	jwt.RegisteredClaims
}
