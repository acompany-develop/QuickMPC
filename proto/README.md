proto
====

# 使用方法

## C++

- `WORKSPACE`

`proto` のワークスペースを参照し、gRPC 利用時に必要な設定を読込・実行する

```bazel
# Enable proto
local_repository(
    name = "proto",
    path = "/proto"
)

load("@proto//bazel:repositories.bzl", "Proto_repositories")
Proto_repositories()
load("@proto//bazel:Proto_deps.bzl", "Proto_deps")
Proto_deps()
load("@proto//bazel:Proto_extra_deps.bzl", "Proto_extra_deps")
Proto_extra_deps()
```

- `BUILD`

依存関係に `@proto//${分類}:${ターゲット名}` を指定する
例: `packages/server/computation_container/server/computation_to_computation_container/BUILD`
```bazel
cc_library(
    ...,
    deps = [
        "@proto//computation_to_computation_container:computation_to_computation_cc_grpc",
    ],
)
```

- `#include` 時

`external/proto/${分類}/${プロトコル名}.grpc.pb.h` のように指定する
例: `packages/server/computation_container/server/computation_to_computation_container/server.hpp`

```cpp
#include "external/proto/computation_to_computation_container/ComputationToComputation.grpc.pb.h"
```

## Go

- `go.mod`

`replace` を利用して対象のプロトコルを `import` できるようにする

```
replace (
	github.com/acompany-develop/QuickMPC/proto/TargetProtocol => ./../proto/TargetProtocol
  ...
)

require (
	github.com/acompany-develop/QuickMPC/proto/TargetProtocol
  ...
)
```

`go mod vendor` を実行し、`go.mod`, `go.sum` を更新する

- `import` 時

`replace` を利用しているため private repository の異なる project を指定できる

```
import (
  ...
  pb "github.com/acompany-develop/QuickMPC/proto/TargetProtocol"
)
```

## docker-compose.yml (develop 用)

`proto` をマウントするように以下を追加する

```yaml
services:
  cc:
    ...
    volumes:
      ...
      - type: bind
        source: ../proto
        target: /proto
```

## Dockerfile (image build 用)

`proto` を利用できるようコピーするため以下を追加する

```docker
COPY proto/ /proto
RUN true
```

# 追加・更新時

## .proto 追加

`proto/${分類}/${プロトコル名}.proto` のように新規ファイルを作成する

## C++ 周りのビルド設定を定義

`proto/${分類}/BUILD` を作成し、以下のように記述

```bazel
load("@rules_proto//proto:defs.bzl", "proto_library")
load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_proto_library")
load("@com_github_grpc_grpc//bazel:cc_grpc_library.bzl", "cc_grpc_library")

# grpc関連のコード生成
proto_library(
    name = "target_protocol",
    srcs = ["TargetProtocol.proto"],
)

cc_proto_library(
    name = "target_protocol_cc_proto",
    deps = [":target_protocol"],
)

cc_grpc_library(
    name = "target_protocol_cc_grpc",
    srcs = [":target_protocol"],
    grpc_only = True,
    deps = [
        ":target_protocol_cc_proto",
        "@com_github_grpc_grpc//:grpc++",
    ],
    visibility = ["//visibility:public"],
)
```

## Go

### Go 用の gRPC コードを生成

.proto ファイル内で `go_package` option を指定

```protobuf
option go_package = "github.com/acompany-develop/QuickMPC/proto/TargetProtocol";
```

以下のコマンドにて生成後、生成物を commit (git の管理下に置く)

```bash
protoc --go_out=. --go_opt=paths=source_relative \
    --go-grpc_out=. --go-grpc_opt=paths=source_relative \
    --proto_path=.:.. \
    ./TargetProtocol.proto
```

生成方法はアップデートにより変更される可能性がある

### `go.mod` 作成

```
module github.com/acompany-develop/QuickMPC/proto/manage_to_computation_container

go 1.18
```

## ワンコマンドでの生成
`proto/`で以下のコマンドを実行することでGoとPythonのコードの作成が可能
```sh
make generate
```
特定のディレクトリを指定して実行したい場合は以下のようにする

### オプション
特定のディレクトリをを指定する場合は `t=dir` として指定する
```sh
# proto/libc_to_manage_container/ 直下のprotoのgRPCコードを生成したい場合
make generate t=./lib_to_manage_container/ 
```
