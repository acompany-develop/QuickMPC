##### Protobuf Rules for Bazel
##### See https://github.com/bazelbuild/rules_proto

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

def Proto_repositories():
    if not native.existing_rule("rules_proto"):
        http_archive(
            name = "rules_proto",
            sha256 = "66bfdf8782796239d3875d37e7de19b1d94301e8972b3cbd2446b332429b4df1",
            strip_prefix = "rules_proto-4.0.0",
            urls = [
                "https://github.com/bazelbuild/rules_proto/archive/refs/tags/4.0.0.tar.gz",
            ],
        )

    if not native.existing_rule("com_github_grpc_grpc"):
        http_archive(
            name = "com_github_grpc_grpc",
            sha256 = "e18b16f7976aab9a36c14c38180f042bb0fd196b75c9fd6a20a2b5f934876ad6",
            urls = [
                "https://github.com/grpc/grpc/archive/refs/tags/v1.45.2.tar.gz",
            ],
            strip_prefix = "grpc-1.45.2",
        )
