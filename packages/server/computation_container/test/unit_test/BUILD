cc_test(
    name = "MathTest",
    srcs = [
        "MathTest.cpp",
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
    name = "ShareTest",
    srcs = [
        "ShareTest.cpp",
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
    name = "RandomTest",
    srcs = [
        "RandomTest.cpp",
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
        "CtoC_test.cpp",
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
    name = "FixedPointTest",
    srcs = [
        "FixedPointTest.cpp",
    ],
    copts = ["-Iexternal/gtest/include"],
    deps = [
        "@gtest//:main",
        "//fixed_point:fixed_point",
        "//fixed_point:math_lib"
    ],
)

cc_test(
    name = "TransactionQueueTest",
    srcs = [
        "TransactionQueueTest.cpp"
    ],
    copts = ["-Iexternal/gtest/include"],
    deps = [
        "@gtest//:main",
       "//transaction_queue:transaction_queue"
     ],
)

cc_test(
    name = "CsprngTest",
    srcs = [
        "CsprngTest.cpp"
    ],
    copts = ["-Iexternal/gtest/include",],
    deps = [
        "@gtest//:main",
        "//random:csprng",
        "//logging:log",
    ],
)

cc_test(
    name = "LogTest",
    srcs = [
        "LogTest.cpp",
    ],
    copts = ["-Iexternal/gtest/include"],
    deps = [
        "@gtest//:main",
        "//logging:log",
    ],
    visibility = ["//visibility:public"],
)

cc_test(
    name = "CtoDBTest",
    srcs = [
        "CtoDBTest.cpp",
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
