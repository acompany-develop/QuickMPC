load("@rules_proto//proto:defs.bzl", "proto_library")
load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_proto_library")
load("@com_github_grpc_grpc//bazel:cc_grpc_library.bzl", "cc_grpc_library")

# grpc関連のコード生成
proto_library(
    name = "computation_to_computation_proto",
    srcs = ["computation_to_computation.proto"],
    deps = ["@com_google_protobuf//:empty_proto",
            "@proto//common_types:common_types"],
)

cc_proto_library(
    name = "computation_to_computation_cc_proto",
    deps = [":computation_to_computation_proto"],
)

cc_grpc_library(
    name = "computation_to_computation_cc_grpc",
    srcs = [":computation_to_computation_proto"],
    grpc_only = True,
    deps = [
        ":computation_to_computation_cc_proto",
        "@com_github_grpc_grpc//:grpc++"
    ],
    visibility = ["//visibility:public"],
)
