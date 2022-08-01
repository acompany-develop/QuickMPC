load("@rules_proto//proto:defs.bzl", "proto_library")
load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_proto_library")
load("@com_github_grpc_grpc//bazel:cc_grpc_library.bzl", "cc_grpc_library")

# grpc関連のコード生成
proto_library(
    name = "engine_to_bts_proto",
    srcs = ["engine_to_bts.proto"],
)

cc_proto_library(
    name = "engine_to_bts_cc_proto",
    deps = [":engine_to_bts_proto"],
)

cc_grpc_library(
    name = "engine_to_bts_cc_grpc",
    srcs = [":engine_to_bts_proto"],
    grpc_only = True,
    deps = [
        ":engine_to_bts_cc_proto",
        "@com_github_grpc_grpc//:grpc++"
    ],
    visibility = ["//visibility:public"],
)
