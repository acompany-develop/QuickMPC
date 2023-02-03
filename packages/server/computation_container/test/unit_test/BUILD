cc_test(
    name = "MathTest",
    srcs = [
        "MathTest.cpp",
    ],
    copts = ["-Iexternal/gtest/include"],
    deps = [
        "@gtest//:main",
        "@com_github_grpc_grpc//:grpc++",
        "//Math:math",
        "//Share:share",
        "//config_parse:config_parse",
        "//Server/ComputationToComputationContainer:server",
        "//Logging:log",
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
        "//Share:share",
        "//config_parse:config_parse",
        "//Server/ComputationToComputationContainer:server",
        "//Logging:log",
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
        "//FixedPoint:fixed_point"
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
        "//Server/ComputationToComputationContainer:server",
        "//Client/Helper:helper",
        "//config_parse:config_parse",
        "@proto//ComputationToComputationContainer:computation_to_computation_cc_grpc",
        "//Share:address",
        "//Logging:log"
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
        "//FixedPoint:fixed_point",
        "//FixedPoint:math_lib"
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
       "//TransactionQueue:transaction_queue"
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
        "//Logging:log",
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
        "//Logging:log",
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
        "//Client/ComputationToDb:client",
        "@proto//common_types:common_types_cc_proto",
    ],
    linkopts = [
        "-lstdc++fs"
    ],
    visibility = ["//visibility:public"],
)
