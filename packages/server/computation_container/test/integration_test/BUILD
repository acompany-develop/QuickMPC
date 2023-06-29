
cc_library(
    name = "share_test",
    srcs = [
        "share_test.hpp",
    ],
    copts = ["-Iexternal/gtest/googletest/include"],
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
    copts = ["-Iexternal/gtest/googletest/include"],
    deps = [
        "@gtest//:main",
        "//triple_handler:triple_handler",
        "//logging:log",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "join_table_test",
    hdrs = [
        "join_table_test.hpp",
    ],
    deps = [
        "@gtest//:main",
        "//value_table:jointable",
    ],
    visibility = ["//visibility:public"],
)
