package(default_visibility = ["//visibility:public"])

cc_binary(
    name = "computation_container",
    deps = [
        "//ComputationParty:computation_party",
    ]
)

cc_binary(
    name = "computation_container_test",
    deps = [
        "//Test/IntegrationTest/ComputationTest:computation_test"
    ]
)

cc_binary(
    name = "computation_container_benchmark",
    deps = [
        "//Test/Benchmark/ComputationBenchmark:computation_benchmark"
    ]
)

# copy file which is generated in subdirectory
# using `cc_binary` does not work
load("@bazel_skylib//rules:copy_file.bzl", "copy_file")

copy_file(
    name = "exception_handler",
    src = "//ExceptionHandler:exception_handler",
    out = "libexception_handler.so",
)

# compile_commands.json を生成
load("@com_grail_bazel_compdb//:defs.bzl", "compilation_database")
load("@com_grail_bazel_output_base_util//:defs.bzl", "OUTPUT_BASE")

compilation_database(
    name = "compdb",
    targets = [
        "//:computation_container_test",
        "//:computation_container",
        "//ExceptionHandler:exception_handler",
    ],
    output_base = OUTPUT_BASE,
)
