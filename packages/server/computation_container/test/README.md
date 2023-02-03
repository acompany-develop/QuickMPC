Testの実行コマンド
---
## test/以下全てのTestを実行したい時
```
bazel test //test/unit_test:all --test_output=streamed
bazel test //test/integration_test:all --test_output=streamed
bazel test //test/integration_test:triple_test --test_output=streamed
bazel build //:all && ./bazel-bin/computation_container_test
```

## triple_recons_test
これは 3Partyで CC を建てる必要があるため現行のLargeでは実行不可能
もし実行する場合は以下のようにすること

- packages/server/computation_container/test/integration_test/computation_test/BUILD
```
cc_library(
    name = "computation_test",
    srcs = [
        "computation_test.cpp"
    ],
    copts = ["-Iexternal/gtest/include"],
    hdrs = [
        "computation_test.hpp",
    ],
    deps = [
        "@gtest//:main",
        "//test/integration_test:share_test",
        "//test/integration_test:share_comp_test",
        "//test/integration_test:math_test",
        "//test/integration_test:matrix_test",
        "//test/integration_test:model_test",
        "//test/integration_test:triple_recons_test",
        "//config_parse:config_parse",
        "//server/computation_to_computation_container:server",
        "//server/manage_to_computation_container:server"
    ],
    visibility = ["//visibility:public"],
)
```

- packages/server/computation_container/test/integration_test/computation_test/computation_test.hpp
```
#pragma once
#include <iostream>
#include <string>
#include <thread>
#include "unistd.h"

#include "config_parse/config_parse.hpp"
#include "server/computation_to_computation_container/server.hpp"
#include "server/manage_to_computation_container/server.hpp"
#include "test/integration_test/ShareCompTest.hpp"
#include "test/integration_test/ShareTest.hpp"
#include "test/integration_test/MathTest.hpp"
#include "test/integration_test/MatrixTest.hpp"
#include "test/integration_test/ModelTest.hpp"
#include "test/integration_test/TripleReconsTest.hpp"

int main(int argc, char **argv);
```
