cc_library(
    name = "computation_benchmark",
    srcs = [
        "ComputationBenchmark.cpp"
    ],
    copts = ["-Iexternal/gtest/include","-DDEBUG"],
    hdrs = [
        "ComputationBenchmark.hpp",
    ],
    deps = [
        "@gtest//:main",
        "//Test/Benchmark:benchmark",
        "//Test/Benchmark:share_benchmark",
        "//Server/ComputationToComputationContainer:server",
        "//Server/ManageToComputationContainer:server",
        "//Logging:log",
        "@com_github_grpc_grpc//proto/grpc/health/v1:health_proto",
    ],
    visibility = ["//visibility:public"],
)
