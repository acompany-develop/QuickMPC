module github.com/acompany-develop/QuickMPC/packages/server/BeaverTripleService

go 1.18

replace github.com/acompany-develop/QuickMPC/proto/EngineToBts => ./../proto/EngineToBts

require (
	github.com/acompany-develop/QuickMPC/proto/EngineToBts v0.0.0-00010101000000-000000000000
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
	go.uber.org/atomic v1.7.0 // indirect
	go.uber.org/multierr v1.6.0 // indirect
	golang.org/x/net v0.4.0 // NOTE: CVE-2022-41717, CVE-2022-41721の回避 indirectだとCVE-2022-41717に引っかかるので決めうち
	golang.org/x/sys v0.0.0-20220412211240-33da011f77ad // NOTE: CVE-2022-29526の回避
	golang.org/x/text v0.3.8 // NOTE: CVE-2020-14040, CVE-2021-38561, CVE-2022-32149の回避
	google.golang.org/genproto v0.0.0-20200526211855-cb27e3aa2013 // indirect
	google.golang.org/protobuf v1.27.1 // indirect
	gopkg.in/check.v1 v1.0.0-20190902080502-41f04d3bba15 // indirect
)
