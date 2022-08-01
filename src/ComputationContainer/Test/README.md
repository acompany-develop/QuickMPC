Testの実行コマンド
---
## Test/以下全てのTestを実行したい時
```
bazel test //Test/UnitTest:all --test_output=streamed
bazel test //Test/IntegrationTest:all --test_output=streamed
bazel test //Test/IntegrationTest:triple_test --test_output=streamed
bazel build //:all && ./bazel-bin/computation_container_test
```

## triple_recons_test
これは 3Partyで CC,DC を建てる必要があるため現行のLargeでは実行不可能
もし実行する場合は以下のようにすること

- src/ComputationContainer/Test/IntegrationTest/ComputationTest/BUILD
```
cc_library(
    name = "computation_test",
    srcs = [
        "ComputationTest.cpp"
    ],
    copts = ["-Iexternal/gtest/include"],
    hdrs = [
        "ComputationTest.hpp",
    ],
    deps = [
        "@gtest//:main",
        "//Test/IntegrationTest:share_test",
        "//Test/IntegrationTest:share_comp_test",
        "//Test/IntegrationTest:math_test",
        "//Test/IntegrationTest:matrix_test",
        "//Test/IntegrationTest:model_test",
        "//Test/IntegrationTest:triple_recons_test",
        "//ConfigParse:config_parse",
        "//Server/ComputationToComputationContainer:server",
        "//Server/ManageToComputationContainer:server"
    ],
    visibility = ["//visibility:public"],
)
```

- src/ComputationContainer/Test/IntegrationTest/ComputationTest/ComputationTest.hpp
```
#pragma once
#include <iostream>
#include <string>
#include <thread>
#include "unistd.h"

#include "ConfigParse/ConfigParse.hpp"
#include "Server/ComputationToComputationContainer/Server.hpp"
#include "Server/ManageToComputationContainer/Server.hpp"
#include "Test/IntegrationTest/ShareCompTest.hpp"
#include "Test/IntegrationTest/ShareTest.hpp"
#include "Test/IntegrationTest/MathTest.hpp"
#include "Test/IntegrationTest/MatrixTest.hpp"
#include "Test/IntegrationTest/ModelTest.hpp"
#include "Test/IntegrationTest/TripleReconsTest.hpp"

int main(int argc, char **argv);
```