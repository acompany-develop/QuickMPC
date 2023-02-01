package main

import (
	l2mserver "github.com/acompany-develop/QuickMPC/packages/server/ManageContainer/Server/LibcToManageContainer"
	m2mserver "github.com/acompany-develop/QuickMPC/packages/server/ManageContainer/Server/ManageToManageContainer"

	manage2comp "github.com/acompany-develop/QuickMPC/packages/server/ManageContainer/Client/ManageToComputationContainer"
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
