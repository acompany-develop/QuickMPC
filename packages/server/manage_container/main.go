package main

import (
	l2mserver "github.com/acompany-develop/QuickMPC/packages/server/manage_container/server/libc_to_manage_container"
	m2mserver "github.com/acompany-develop/QuickMPC/packages/server/manage_container/server/manage_to_manage_container"

	manage2comp "github.com/acompany-develop/QuickMPC/packages/server/manage_container/client/manage_to_computation_container"
)

func main() {
	go func() {
		m2mserver.RunServer()
	}()

	go func() {
		manage2comp.CheckStateOfComputationContainerRegularly()
	}()

	l2mserver.RunServer()
}
