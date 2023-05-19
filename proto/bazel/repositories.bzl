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
