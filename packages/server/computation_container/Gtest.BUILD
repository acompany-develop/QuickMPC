cc_library(
    name = "main",
    srcs = glob(
        ["src/*.cc"],
        exclude = ["src/gtest-all.cc"]
    ),
    hdrs = glob([
        "googletest/include/**/*.h",
        "googletest/src/*.h"
    ]),
    copts = ["-Iexternal/gtest/googletest/include"],
    linkopts = ["-pthread"],
    visibility = ["//visibility:public"],
)
