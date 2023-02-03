cc_library(
    name = "computation_benchmark",
    srcs = [
        "computation_benchmark.cpp"
    ],
    copts = ["-Iexternal/gtest/include","-DDEBUG"],
    hdrs = [
        "computation_benchmark.hpp",
    ],
    deps = [
        "@gtest//:main",
        "//test/benchmark:benchmark",
        "//test/benchmark:share_benchmark",
        "//Server/ComputationToComputationContainer:server",
        "//Server/ManageToComputationContainer:server",
        "//Logging:log",
        "@com_github_grpc_grpc//src/proto/grpc/health/v1:health_proto",
    ],
    visibility = ["//visibility:public"],
)
