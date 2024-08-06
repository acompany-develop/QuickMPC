module github.com/acompany-develop/QuickMPC/proto/engine_to_bts

go 1.18

require (
	github.com/acompany-develop/QuickMPC/proto/common_types v0.0.0-20231203214653-87608b4d6aa5
	golang.org/x/crypto v0.17.0 // NOTE: CVE-2022-27191の回避
	golang.org/x/net v0.17.0 // NOTE: CVE-2022-41717, CVE-2022-41721の回避 indirectだとCVE-2022-41717に引っかかるので決めうち
	golang.org/x/sys v0.15.0 // NOTE: CVE-2022-29526の回避
	golang.org/x/text v0.14.0 // NOTE: CVE-2020-14040, CVE-2021-38561, CVE-2022-32149の回避
	google.golang.org/grpc v1.56.3
	google.golang.org/protobuf v1.33.0
	gopkg.in/yaml.v2 v2.2.8 // NOTE: CVE-2022-3064の回避
)

require (
	github.com/golang/protobuf v1.5.3 // indirect
	google.golang.org/genproto v0.0.0-20230410155749-daa745c078e1 // indirect
)
