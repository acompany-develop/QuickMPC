cc_library(
    name = "sid3",
    hdrs = [
        "SID3.hpp",
    ],
    srcs = [
        "SID3.cpp",
    ],
    deps = [
        "//Share:share",
        "//Math:math",
        "@nlohmann_json//:json",
    ],
    visibility = ["//visibility:public"],
)
cc_library(
    name = "sid3reg",
    hdrs = [
        "SID3Regression.hpp",
    ],
    srcs = [
        "SID3Regression.cpp",
    ],
    deps = [
        "//Share:share",
        "//Math:math",
        "@nlohmann_json//:json",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "sgbm",
    hdrs = [
        "SGBM.hpp",
    ],
    srcs = [
    ],
    deps = [
        "//Share:share",
        "//Math:math",
        "//GBDT:sid3reg",
        "@nlohmann_json//:json",
    ],
    visibility = ["//visibility:public"],

)
cc_library(
    name = "gbdt",
    hdrs = [
        "SID3.hpp",
        "SID3Regression.hpp",
    ],
    srcs = [
    ],
    deps = [
        "//GBDT:sid3",
        "//GBDT:sid3reg",
        "//GBDT:sgbm",
    ],
    visibility = ["//visibility:public"],
)