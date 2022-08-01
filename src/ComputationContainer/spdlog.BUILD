load("@rules_cc//cc:defs.bzl", "cc_library")

licenses(["notice"])  # Apache 2

cc_library(
    name = "spdlog",
    hdrs = glob([
        "spdlog/**"
    ]),
    includes = ["."],
    visibility = ["//visibility:public"],
)