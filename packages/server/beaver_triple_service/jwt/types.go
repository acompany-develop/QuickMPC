package jwt

import "github.com/golang-jwt/jwt/v4"

type Party struct {
	Id      int    `json:"id"`
	Address string `json:"address"`
}

type Claim struct {
	RoomUUID  string  `json:"room_uuid"`
	PartyInfo []Party `json:"party_info"`
	jwt.RegisteredClaims
}
