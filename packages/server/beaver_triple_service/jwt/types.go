package jwt

import "github.com/golang-jwt/jwt/v4"

type Party struct {
	Id      uint32 `json:"id"`
	Address string `json:"address"`
}

type Claim struct {
	RoomUUID  string  `json:"room_uuid"`
	PartyId uint32 `json:"party_id"`
	PartyInfo []Party `json:"party_info"`
	Iat  int64  `json:"room_uuid,omitempty"`
	Nbf int64 `json:"party_id,omitempty"`
	Exp int64 `json:"party_info,omitempty"`
	jwt.RegisteredClaims
}
