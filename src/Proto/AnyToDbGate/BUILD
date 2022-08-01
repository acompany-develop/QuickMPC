load("@rules_proto//proto:defs.bzl", "proto_library")
load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_proto_library")
load("@com_github_grpc_grpc//bazel:cc_grpc_library.bzl", "cc_grpc_library")

# grpc関連のコード生成
proto_library(
    name = "any_to_db_gate_proto",
    srcs = ["any_to_db_gate.proto"],
)

cc_proto_library(
    name = "any_to_db_gate_cc_proto",
    deps = [":any_to_db_gate_proto"],
)

cc_grpc_library(
    name = "any_to_db_gate_cc_grpc",
    srcs = [":any_to_db_gate_proto"],
    grpc_only = True,
    deps = [
        ":any_to_db_gate_cc_proto",
        "@com_github_grpc_grpc//:grpc++",
    ],
    visibility = ["//visibility:public"],
)
