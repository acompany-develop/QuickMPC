cc_library(
    name = "computation_test",
    srcs = [
        "computation_test.cpp"
    ],
    copts = ["-Iexternal/gtest/include","-DDEBUG"],
    hdrs = [
        "computation_test.hpp",
    ],
    deps = [
        "@gtest//:main",
        "//test/integration_test:share_test",
        "//test/integration_test:meshcode_test",
        "//test/integration_test:math_test",
        "//test/integration_test:matrix_test",
        "//test/integration_test:read_triple_test",
        "//test/integration_test:value_table_test",
        "//config_parse:config_parse",
        "//Server/ComputationToComputationContainer:server",
        "//Server/ManageToComputationContainer:server",
        "@com_github_grpc_grpc//src/proto/grpc/health/v1:health_proto",
        "//Logging:log",
    ],
    visibility = ["//visibility:public"],
)
