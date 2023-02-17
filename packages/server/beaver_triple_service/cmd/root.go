package cmd

import (
	"os"

	e2bserver "github.com/acompany-develop/QuickMPC/packages/server/beaver_triple_service/server/engine_to_bts"

	"github.com/spf13/cobra"
)

var rootCmd = &cobra.Command{
	Use:   "beaver_triple_service",
	Short: "Start the beaver_triple_service server",
	Run: func(cmd *cobra.Command, args []string) {
		e2bserver.RunServer()
	},
}

func Execute() {
	err := rootCmd.Execute()
	if err != nil {
		os.Exit(1)
	}
}
