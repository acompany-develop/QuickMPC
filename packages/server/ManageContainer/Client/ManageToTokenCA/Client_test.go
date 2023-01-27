package mng2tca

import (
	"testing"
)

const tokenError = "token_error"

var c = Client{}

func TestAuthDep(t *testing.T) {
	err := c.AuthorizeDep(authTokenDep)
	if err != nil {
		t.Error(err)
	}
}
func TestAuthDemo(t *testing.T) {
	errDep := c.AuthorizeDemo(authTokenDemo)
	if errDep != nil {
		t.Error(errDep)
	}
}
func TestAuthDepFail(t *testing.T) {
	err := c.AuthorizeDep(tokenError)
	if err == nil {
		t.Errorf("token: %s must not authorized", tokenError)
	}
}
func TestAuthDemoFail(t *testing.T) {
	err := c.AuthorizeDemo(tokenError)
	if err == nil {
		t.Errorf("token: %s must not authorized", tokenError)
	}
}
