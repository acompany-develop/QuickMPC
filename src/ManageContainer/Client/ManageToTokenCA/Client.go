package mng2tca

import (
	"errors"
)

// 認証局がないので埋め込みして文字列一致判定するだけ
const authTokenDep = "token_dep"
const authTokenDemo = "token_demo"

type Client struct{}
type M2TCAClient interface {
	AuthorizeDep(string) error
	AuthorizeDemo(string) error
}

func authorize(token string, authToken string) error {
	if token != authToken {
		return errors.New("access token was not authorized")
	}
	return nil
}

func (Client) AuthorizeDep(token string) error {
	return authorize(token, authTokenDep)
}

func (Client) AuthorizeDemo(token string) error {
	return authorize(token, authTokenDemo)
}
