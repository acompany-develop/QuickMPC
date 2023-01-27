load("@rules_proto//proto:repositories.bzl", "rules_proto_dependencies", "rules_proto_toolchains")
load("@com_github_grpc_grpc//bazel:grpc_deps.bzl", "grpc_deps")

def Proto_deps():
    rules_proto_dependencies()
    rules_proto_toolchains()
    grpc_deps()
