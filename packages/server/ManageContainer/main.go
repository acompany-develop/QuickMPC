package main

import (
	l2mserver "github.com/acompany-develop/QuickMPC/src/ManageContainer/Server/LibcToManageContainer"
	m2mserver "github.com/acompany-develop/QuickMPC/src/ManageContainer/Server/ManageToManageContainer"

	manage2comp "github.com/acompany-develop/QuickMPC/src/ManageContainer/Client/ManageToComputationContainer"
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
