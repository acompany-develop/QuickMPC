cc_library(
    name = "benchmark",
    srcs = [
        "benchmark.hpp",
    ],
    deps =[
        "//Logging:log",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "share_benchmark",
    srcs = [
        "ShareBenchmark.hpp",
    ],
    copts = ["-Iexternal/gtest/include"],
    deps = [
        "@gtest//:main",
        "//random:random",
        "//Share:compare",
    ],
    visibility = ["//visibility:public"],
)
