module github.com/acompany-develop/QuickMPC/packages/server/beaver_triple_service

go 1.18

replace (
	github.com/acompany-develop/QuickMPC/proto/common_types => ./../proto/common_types
	github.com/acompany-develop/QuickMPC/proto/engine_to_bts => ./../proto/engine_to_bts
)

require (
	github.com/acompany-develop/QuickMPC/proto/common_types v0.0.0-00010101000000-000000000000
	github.com/acompany-develop/QuickMPC/proto/engine_to_bts v0.0.0-00010101000000-000000000000
	github.com/golang-jwt/jwt/v4 v4.4.2
	github.com/google/uuid v1.3.0
	github.com/grpc-ecosystem/go-grpc-middleware v1.3.0
	go.uber.org/zap v1.21.0
	google.golang.org/grpc v1.42.0
	gopkg.in/yaml.v3 v3.0.1
)

require (
	github.com/golang/protobuf v1.5.0 // indirect
	github.com/google/go-cmp v0.5.6 // indirect
	github.com/inconshreveable/mousetrap v1.1.0 // indirect
	github.com/spf13/cobra v1.6.1
	github.com/spf13/pflag v1.0.5 // indirect
	go.uber.org/atomic v1.7.0 // indirect
	go.uber.org/multierr v1.6.0 // indirect
	golang.org/x/net v0.7.0 // indirect; NOTE: CVE-2022-41717, CVE-2022-41721の回避 indirectだとCVE-2022-41717に引っかかるので決めうち
	golang.org/x/sys v0.5.0 // indirect; NOTE: CVE-2022-29526の回避
	golang.org/x/text v0.7.0 // indirect; NOTE: CVE-2020-14040, CVE-2021-38561, CVE-2022-32149の回避
	google.golang.org/genproto v0.0.0-20200526211855-cb27e3aa2013 // indirect
	google.golang.org/protobuf v1.27.1
	gopkg.in/check.v1 v1.0.0-20190902080502-41f04d3bba15 // indirect
)
