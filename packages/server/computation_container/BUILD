package(default_visibility = ["//visibility:public"])

platform(
    name = "docker_image_platform",
    constraint_values = [
        "@bazel_tools//platforms:x86_64",
        "@bazel_tools//platforms:linux",
        "@bazel_tools//tools/cpp:clang",
    ],
    exec_properties = {
        "OSFamily": "Linux",
        "container-image": "docker://ghcr.io/acompany-develop/quickmpc-cc:0.3.8",
    },
)

cc_binary(
    name = "computation_container",
    deps = [
        "//computation_party:computation_party",
    ]
)

cc_binary(
    name = "computation_container_test",
    deps = [
        "//test/integration_test/computation_test:computation_test"
    ]
)

cc_binary(
    name = "computation_container_benchmark",
    deps = [
        "//test/benchmark/computation_benchmark:computation_benchmark"
    ]
)

# compile_commands.json を生成
load("@com_grail_bazel_compdb//:defs.bzl", "compilation_database")
load("@com_grail_bazel_output_base_util//:defs.bzl", "OUTPUT_BASE")

compilation_database(
    name = "compdb",
    targets = [
        "//:computation_container_test",
        "//:computation_container",
    ],
    output_base = OUTPUT_BASE,
)
