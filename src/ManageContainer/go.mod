module github.com/acompany-develop/QuickMPC/src/ManageContainer

go 1.14

replace (
	github.com/acompany-develop/QuickMPC/src/Proto/LibcToManageContainer => ./../Proto/LibcToManageContainer
	github.com/acompany-develop/QuickMPC/src/Proto/ManageToComputationContainer => ./../Proto/ManageToComputationContainer
	github.com/acompany-develop/QuickMPC/src/Proto/ManageToManageContainer => ./../Proto/ManageToManageContainer
	github.com/acompany-develop/QuickMPC/src/Proto/common_types => ./../Proto/common_types
)

require (
	github.com/acompany-develop/QuickMPC/src/Proto/LibcToManageContainer v0.0.0-00010101000000-000000000000
	github.com/acompany-develop/QuickMPC/src/Proto/ManageToComputationContainer v0.0.0-00010101000000-000000000000
	github.com/acompany-develop/QuickMPC/src/Proto/ManageToManageContainer v0.0.0-00010101000000-000000000000
	github.com/acompany-develop/QuickMPC/src/Proto/common_types v0.0.0-00010101000000-000000000000
	github.com/golang/protobuf v1.5.0
	github.com/google/go-cmp v0.5.7 // indirect
	github.com/google/uuid v1.1.2
	github.com/kr/pretty v0.3.0 // indirect
	github.com/pkg/errors v0.9.1 // indirect
	github.com/rogpeppe/go-internal v1.8.1 // indirect
	go.uber.org/atomic v1.9.0 // indirect
	go.uber.org/multierr v1.8.0 // indirect
	go.uber.org/zap v1.21.0
	golang.org/x/net v0.0.0-20220909164309-bea034e7d591 // indirect
	golang.org/x/text v0.3.8 // indirect
	google.golang.org/genproto v0.0.0-20200526211855-cb27e3aa2013
	google.golang.org/grpc v1.41.0
	google.golang.org/protobuf v1.28.1
	gopkg.in/check.v1 v1.0.0-20190902080502-41f04d3bba15 // indirect
)

exclude (
	// NOTE: CVE-2021-44716 の回避
	// もっといい方法あると思うんだけどとりあえず　v0.0.0-20211015210444-4f30a5c0130f 未満を片っ端からexcludeしても良さそうなので今はこうしておく
	golang.org/x/net v0.0.0-20180724234803-3673e40ba225
	golang.org/x/net v0.0.0-20180826012351-8a410e7b638d
	golang.org/x/net v0.0.0-20190108225652-1e06a53dbb7e
	golang.org/x/net v0.0.0-20190213061140-3a22650c66bd
	golang.org/x/net v0.0.0-20190311183353-d8887717615a
	golang.org/x/net v0.0.0-20190404232315-eb5bcb51f2a3
	golang.org/x/net v0.0.0-20190620200207-3b0461eec859
	golang.org/x/net v0.0.0-20200822124328-c89045814202
	golang.org/x/net v0.0.0-20201021035429-f5854403a974
	golang.org/x/net v0.0.0-20210226172049-e18ecbb05110
	golang.org/x/net v0.0.0-20210405180319-a5a99cb37ef4
	golang.org/x/net v0.0.0-20211015210444-4f30a5c0130f
)
