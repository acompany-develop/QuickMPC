cc_library(
    name = "benchmark",
    srcs = [
        "Benchmark.hpp",
    ],
    deps =[
        "//LogHeader:logger",
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
        "//Random:random",
        "//Share:compare",
    ],
    visibility = ["//visibility:public"],
)
