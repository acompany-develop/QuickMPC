
cc_library(
    name = "ot",
    srcs = [
        "OT.cpp","Group.cpp"
    ],
    hdrs=["OT.hpp","Group.hpp"],
    copts = ["-Iexternal/gtest/include"],
    linkopts=["-lboost_system", "-lboost_thread","-lboost_serialization","-lcrypto"],
    deps = [
        "@gtest//:main",
        "//ConfigParse:config_parse",
        "@libsodium//:sodium",
    ],
    visibility = ["//visibility:public"],
)