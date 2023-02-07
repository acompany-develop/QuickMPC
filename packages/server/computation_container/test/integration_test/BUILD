
cc_library(
    name = "share_test",
    srcs = [
        "share_test.hpp",
    ],
    copts = ["-Iexternal/gtest/include"],
    deps = [
        "@gtest//:main",
        "//random:random",
        "//share:compare",
        "//logging:log"
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "math_test",
    srcs = [
        "math_test.hpp",
    ],
    deps = [
        "@gtest//:main",
        "//math:math",
        "//logging:log"
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "matrix_test",
    hdrs = [
        "matrix_test.hpp",
    ],
    deps = [
        "@gtest//:main",
        "//share:matrix",
        "//logging:log",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "meshcode_test",
    srcs = [
        "mesh_code_test.hpp",
    ],
    deps = [
        "@gtest//:main",
        "//share:share",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "read_triple_test",
    srcs = [
        "read_triple_from_bts_test.hpp",
    ],
    copts = ["-Iexternal/gtest/include"],
    deps = [
        "@gtest//:main",
        "//triple_handler:triple_handler",
        "//logging:log",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "value_table_test",
    hdrs = [
        "value_table_test.hpp",
    ],
    deps = [
        "@gtest//:main",
        "//client/computation_to_db:valuetable",
    ],
    visibility = ["//visibility:public"],
)