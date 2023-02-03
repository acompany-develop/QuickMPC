cc_test(
    name = "math_test",
    srcs = [
        "math_test.cpp",
    ],
    copts = ["-Iexternal/gtest/include"],
    deps = [
        "@gtest//:main",
        "@com_github_grpc_grpc//:grpc++",
        "//math:math",
        "//share:share",
        "//config_parse:config_parse",
        "//server/computation_to_computation_container:server",
        "//logging:log",
    ],
    visibility = ["//visibility:public"],
)

cc_test(
    name = "share_test",
    srcs = [
        "share_test.cpp",
    ],
    copts = ["-Iexternal/gtest/include"],
    deps = [
        "@gtest//:main",
        "@com_github_grpc_grpc//:grpc++",
        "//share:share",
        "//config_parse:config_parse",
        "//server/computation_to_computation_container:server",
        "//logging:log",
    ],
)

cc_test(
    name = "random_test",
    srcs = [
        "random_test.cpp",
    ],
    copts = ["-Iexternal/gtest/include"],
    deps = [
        "@gtest//:main",
        "//random:random",
        "//fixed_point:fixed_point"
    ],
)

cc_test(
    name = "CtoCTest",
    srcs = [
        "ctoc_test.cpp",
    ],
    copts = ["-Iexternal/gtest/include"],
    deps = [
        "@gtest//:main",
        "//server/computation_to_computation_container:server",
        "//client/helper:helper",
        "//config_parse:config_parse",
        "@proto//computation_to_computation_container:computation_to_computation_cc_grpc",
        "//share:address",
        "//logging:log"
    ],
    linkopts = ["-pthread"],
)

cc_test(
    name = "fixed_point_test",
    srcs = [
        "fixed_point_test.cpp",
    ],
    copts = ["-Iexternal/gtest/include"],
    deps = [
        "@gtest//:main",
        "//fixed_point:fixed_point",
        "//fixed_point:math_lib"
    ],
)

cc_test(
    name = "transaction_queue_test",
    srcs = [
        "transaction_queue_test.cpp"
    ],
    copts = ["-Iexternal/gtest/include"],
    deps = [
        "@gtest//:main",
       "//transaction_queue:transaction_queue"
     ],
)

cc_test(
    name = "csprng_test",
    srcs = [
        "csprng_test.cpp"
    ],
    copts = ["-Iexternal/gtest/include",],
    deps = [
        "@gtest//:main",
        "//random:csprng",
        "//logging:log",
    ],
)

cc_test(
    name = "log_test",
    srcs = [
        "log_test.cpp",
    ],
    copts = ["-Iexternal/gtest/include"],
    deps = [
        "@gtest//:main",
        "//logging:log",
    ],
    visibility = ["//visibility:public"],
)

cc_test(
    name = "ctodb_test",
    srcs = [
        "ctodb_test.cpp",
    ],
    copts = ["-Iexternal/gtest/include"],
    deps = [
        "@gtest//:main",
        "//client/computation_to_db:client",
        "@proto//common_types:common_types_cc_proto",
    ],
    linkopts = [
        "-lstdc++fs"
    ],
    visibility = ["//visibility:public"],
)
