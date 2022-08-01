load("@rules_proto//proto:defs.bzl", "proto_library")
load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_proto_library")
load("@com_github_grpc_grpc//bazel:cc_grpc_library.bzl", "cc_grpc_library")

# grpc関連のコード生成
proto_library(
    name = "manage_to_computation_proto",
    srcs = ["manage_to_computation.proto"],
    deps = ["@com_google_protobuf//:empty_proto",
            "@Proto//common_types:common_types"],
    visibility = ["//visibility:public"],
)

cc_proto_library(
    name = "manage_to_computation_cc_proto",
    deps = [":manage_to_computation_proto"],
)

cc_grpc_library(
    name = "manage_to_computation_cc_grpc",
    srcs = [":manage_to_computation_proto"],
    grpc_only = True,
    deps = [
        ":manage_to_computation_cc_proto",
        "@com_github_grpc_grpc//:grpc++"
    ],
    visibility = ["//visibility:public"],
)
