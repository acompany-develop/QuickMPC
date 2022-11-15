
cc_test(
    name = "OT",
    srcs = [
        "OT.cpp",
    ],
    hdrs = ["OT.hpp"],
    copts = ["-Iexternal/gtest/include"],
    deps = [
        "@libOTe//:OTe",
        "//Share:compare",
    ],
    visibility = ["//visibility:public"],
)