module github.com/acompany-develop/QuickMPC/src/DbContainer/DbGate

go 1.14

replace github.com/acompany-develop/QuickMPC/src/Proto/AnyToDbGate => ./../Proto/AnyToDbGate

require (
	github.com/acompany-develop/QuickMPC/src/Proto/AnyToDbGate v0.0.0-00010101000000-000000000000
	github.com/couchbase/gocb/v2 v2.3.2
	github.com/google/uuid v1.1.2
	go.uber.org/atomic v1.9.0 // indirect
	go.uber.org/multierr v1.8.0 // indirect
	go.uber.org/zap v1.21.0
	golang.org/x/net v0.0.0-20220708220712-1185a9018129 // indirect
	google.golang.org/genproto v0.0.0-20210927142257-433400c27d05
	google.golang.org/grpc v1.41.0
	gopkg.in/yaml.v2 v2.4.0 // indirect
)

exclude (
	// NOTE: CVE-2021-33194 の回避のために指定している
	golang.org/x/net v0.0.0-20180724234803-3673e40ba225
	golang.org/x/net v0.0.0-20180826012351-8a410e7b638d
	golang.org/x/net v0.0.0-20190108225652-1e06a53dbb7e
	golang.org/x/net v0.0.0-20190213061140-3a22650c66bd
	golang.org/x/net v0.0.0-20190311183353-d8887717615a
	golang.org/x/net v0.0.0-20190404232315-eb5bcb51f2a3
	golang.org/x/net v0.0.0-20190620200207-3b0461eec859
	golang.org/x/net v0.0.0-20200822124328-c89045814202
	golang.org/x/net v0.0.0-20210405180319-a5a99cb37ef4

	// NOTE: CVE-2021-38561 の回避のために指定している
	golang.org/x/text v0.3.0
	golang.org/x/text v0.3.3
	golang.org/x/text v0.3.5
)
