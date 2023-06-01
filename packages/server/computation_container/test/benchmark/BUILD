cc_library(
    name = "benchmark",
    srcs = [
        "benchmark.hpp",
    ],
    deps =[
        "//logging:log",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "share_benchmark",
    srcs = [
        "share_benchmark.hpp",
    ],
    copts = ["-Iexternal/gtest/googletest/include"],
    deps = [
        "@gtest//:main",
        "//random:random",
        "//share:compare",
    ],
    visibility = ["//visibility:public"],
)
