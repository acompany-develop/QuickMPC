module github.com/acompany-develop/QuickMPC/proto/engine_to_bts

go 1.18

require (
	google.golang.org/grpc v1.42.0
	google.golang.org/protobuf v1.27.1
	golang.org/x/crypto v0.0.0-20220314234659-1baeb1ce4c0b // NOTE: CVE-2022-27191の回避
	golang.org/x/net v0.4.0 // NOTE: CVE-2022-41717, CVE-2022-41721の回避 indirectだとCVE-2022-41717に引っかかるので決めうち
	golang.org/x/text v0.3.8 // NOTE: CVE-2020-14040, CVE-2021-38561, CVE-2022-32149の回避
	golang.org/x/sys v0.0.0-20220412211240-33da011f77ad // NOTE: CVE-2022-29526の回避
	gopkg.in/yaml.v2 v2.2.8 // NOTE: CVE-2022-3064の回避
)
