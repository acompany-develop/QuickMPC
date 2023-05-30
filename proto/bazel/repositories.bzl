##### Protobuf Rules for Bazel
##### See https://github.com/bazelbuild/rules_proto

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

def Proto_repositories():
    if not native.existing_rule("rules_proto"):
        http_archive(
            name = "rules_proto",
            sha256 = "c22cfcb3f22a0ae2e684801ea8dfed070ba5bed25e73f73580564f250475e72d",
            strip_prefix = "rules_proto-4.0.0-3.19.2",
            urls = [
                "https://github.com/bazelbuild/rules_proto/archive/refs/tags/4.0.0-3.19.2.tar.gz",
            ],
        )

    if not native.existing_rule("com_github_grpc_grpc"):
        http_archive(
            name = "com_github_grpc_grpc",
            sha256 = "9cf1a69a921534ac0b760dcbefb900f3c2f735f56070bf0536506913bb5bfd74",
            urls = [
                "https://github.com/grpc/grpc/archive/refs/tags/v1.55.0.tar.gz",
            ],
            strip_prefix = "grpc-1.55.0",
        )
