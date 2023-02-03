
cc_library(
    name = "share_test",
    srcs = [
        "ShareTest.hpp",
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
        "MathTest.hpp",
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
        "MatrixTest.hpp",
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
        "MeshCodeTest.hpp",
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
        "ReadTripleFromBtsTest.hpp",
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
        "ValueTableTest.hpp",
    ],
    deps = [
        "@gtest//:main",
        "//client/computation_to_db:valuetable",
    ],
    visibility = ["//visibility:public"],
)
