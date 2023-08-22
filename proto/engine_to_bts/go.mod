module github.com/acompany-develop/QuickMPC/proto/engine_to_bts

go 1.18

require (
	golang.org/x/net v0.7.0 // indirect; NOTE: CVE-2022-41717, CVE-2022-41721の回避 indirectだとCVE-2022-41717に引っかかるので決めうち
	golang.org/x/sys v0.5.0 // indirect; NOTE: CVE-2022-29526の回避
	golang.org/x/text v0.7.0 // indirect; NOTE: CVE-2020-14040, CVE-2021-38561, CVE-2022-32149の回避
	google.golang.org/grpc v1.42.0
	google.golang.org/protobuf v1.27.1
)

require (
	github.com/golang/protobuf v1.5.0 // indirect
	google.golang.org/genproto v0.0.0-20200526211855-cb27e3aa2013 // indirect
)
