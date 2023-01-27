load("@rules_proto//proto:defs.bzl", "proto_library")
load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_proto_library")

# grpc関連のコード生成
proto_library(
    name = "common_types",
    srcs = ["common_types.proto"],
    visibility = ["//visibility:public"],
)

cc_proto_library(
    name = "common_types_cc_proto",
    deps = [":common_types"],
    visibility = ["//visibility:public"],
)
