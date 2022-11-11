cc_library(
    name = "computation_test",
    srcs = [
        "ComputationTest.cpp"
    ],
    copts = ["-Iexternal/gtest/include","-DDEBUG"],
    hdrs = [
        "ComputationTest.hpp",
    ],
    deps = [
        "@gtest//:main",
        "//Test/IntegrationTest:share_test",
        "//Test/IntegrationTest:meshcode_test",
        "//Test/IntegrationTest:math_test",
        "//Test/IntegrationTest:matrix_test",
        "//Test/IntegrationTest:model_test",
        "//Test/IntegrationTest:optimize_test",
        "//Test/IntegrationTest:read_triple_test",
        "//Test/IntegrationTest:gbdt_test",
        "//Test/IntegrationTest:value_table_test",
        "//ConfigParse:config_parse",
        "//Server/ComputationToComputationContainer:server",
        "//Server/ManageToComputationContainer:server",
        "@com_github_grpc_grpc//src/proto/grpc/health/v1:health_proto",
        "//Logging:log",
    ],
    visibility = ["//visibility:public"],
)
