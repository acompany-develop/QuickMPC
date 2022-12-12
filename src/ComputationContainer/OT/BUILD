
cc_library(
    name = "ot",
    srcs = [
        "OT.cpp",
    ],
    hdrs=["OT.hpp"],
    copts = ["-Iexternal/gtest/include"],
    linkopts=["-lboost_system", "-lboost_thread","-lboost_serialization"],
    deps = [
        "@gtest//:main",
        "//ConfigParse:config_parse",
        "@libsodium//:sodium",
    ],
    visibility = ["//visibility:public"],
)