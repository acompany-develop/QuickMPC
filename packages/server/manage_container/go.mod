module github.com/acompany-develop/QuickMPC/packages/server/manage_container

go 1.18

replace (
	github.com/acompany-develop/QuickMPC/proto/common_types => ./../proto/common_types
	github.com/acompany-develop/QuickMPC/proto/libc_to_manage_container => ./../proto/libc_to_manage_container
	github.com/acompany-develop/QuickMPC/proto/manage_to_computation_container => ./../proto/manage_to_computation_container
	github.com/acompany-develop/QuickMPC/proto/manage_to_manage_container => ./../proto/manage_to_manage_container
)

require (
	github.com/acompany-develop/QuickMPC/proto/common_types v0.0.0-20231203214653-87608b4d6aa5
	github.com/acompany-develop/QuickMPC/proto/libc_to_manage_container v0.0.0-00010101000000-000000000000
	github.com/acompany-develop/QuickMPC/proto/manage_to_computation_container v0.0.0-00010101000000-000000000000
	github.com/acompany-develop/QuickMPC/proto/manage_to_manage_container v0.0.0-00010101000000-000000000000
	github.com/golang/protobuf v1.5.0
	github.com/google/uuid v1.1.2
	go.uber.org/zap v1.21.0
	google.golang.org/grpc v1.56.3
	google.golang.org/protobuf v1.28.1
)

require (
	github.com/google/go-cmp v0.5.7 // indirect
	github.com/pkg/errors v0.9.1 // indirect
	go.uber.org/atomic v1.9.0 // indirect
	go.uber.org/multierr v1.8.0 // indirect
	golang.org/x/net v0.7.0 // indirect; NOTE: CVE-2022-41717, CVE-2022-41721の回避 indirectだとCVE-2022-41717に引っかかるので決めうち
	golang.org/x/sys v0.5.0 // indirect
	golang.org/x/text v0.7.0 // indirect
	google.golang.org/genproto v0.0.0-20200526211855-cb27e3aa2013 // indirect
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
