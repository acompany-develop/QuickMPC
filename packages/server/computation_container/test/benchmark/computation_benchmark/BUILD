cc_library(
    name = "computation_benchmark",
    srcs = [
        "computation_benchmark.cpp"
    ],
    copts = ["-Iexternal/gtest/googletest/include","-DDEBUG"],
    hdrs = [
        "computation_benchmark.hpp",
    ],
    deps = [
        "@gtest//:main",
        "//test/benchmark:benchmark",
        "//test/benchmark:share_benchmark",
        "//server/computation_to_computation_container:server",
        "//server/manage_to_computation_container:server",
        "//logging:log",
        "@com_github_grpc_grpc//src/proto/grpc/health/v1:health_proto",
    ],
    visibility = ["//visibility:public"],
)
