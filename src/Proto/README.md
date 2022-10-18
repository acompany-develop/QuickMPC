Proto
====

# 使用方法

## C++

- `WORKSPACE`

`src/Proto` のワークスペースを参照し、gRPC 利用時に必要な設定を読込・実行する

```bazel
# Enable Proto
local_repository(
    name = "Proto",
    path = "/Proto"
)

load("@Proto//bazel:repositories.bzl", "Proto_repositories")
Proto_repositories()
load("@Proto//bazel:Proto_deps.bzl", "Proto_deps")
Proto_deps()
load("@Proto//bazel:Proto_extra_deps.bzl", "Proto_extra_deps")
Proto_extra_deps()
```

- `BUILD`

依存関係に `@Proto//${分類}:${ターゲット名}` を指定する
例: `src/ComputationContainer/Server/ComputationToComputationContainer/BUILD`
```bazel
cc_library(
    ...,
    deps = [
        "@Proto//ComputationToComputationContainer:computation_to_computation_cc_grpc",
    ],
)
```

- `#include` 時

`external/Proto/${分類}/${プロトコル名}.grpc.pb.h` のように指定する
例: `src/ComputationContainer/Server/ComputationToComputationContainer/Server.hpp`

```cpp
#include "external/Proto/ComputationToComputationContainer/ComputationToComputation.grpc.pb.h"
```

## Go

- `go.mod`

`replace` を利用して対象のプロトコルを `import` できるようにする

```
replace (
	github.com/acompany-develop/QuickMPC/src/Proto/TargetProtocol => ./../Proto/TargetProtocol
  ...
)

require (
	github.com/acompany-develop/QuickMPC/src/Proto/TargetProtocol
  ...
)
```

`go mod vendor` を実行し、`go.mod`, `go.sum` を更新する

- `import` 時

`replace` を利用しているため private repository の異なる project を指定できる

```
import (
  ...
  pb "github.com/acompany-develop/QuickMPC/src/Proto/TargetProtocol"
)
```

## docker-compose.yml (develop 用)

`src/Proto` をマウントするように以下を追加する

```yaml
services:
  cc:
    ...
    volumes:
      ...
      - type: bind
        source: ../Proto
        target: /Proto
```

## Dockerfile (image build 用)

`src/Proto` を利用できるようコピーするため以下を追加する

```docker
COPY src/Proto/ /Proto
RUN true
```

# 追加・更新時

## .proto 追加

`src/Proto/${分類}/${プロトコル名}.proto` のように新規ファイルを作成する

## C++ 周りのビルド設定を定義

`src/Proto/${分類}/BUILD` を作成し、以下のように記述

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
option go_package = "github.com/acompany-develop/QuickMPC/src/Proto/TargetProtocol";
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
module github.com/acompany-develop/QuickMPC/src/Proto/ManageToComputationContainer

go 1.14
```
