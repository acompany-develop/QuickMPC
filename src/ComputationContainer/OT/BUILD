
cc_test(
    name = "OTTest",
    srcs = [
        "OTTest.cpp",
    ],
    copts = ["-Iexternal/gtest/include"],
    linkopts=["-lboost_system", "-lboost_thread"],
    deps = [
        "@gtest//:main",
        "//ConfigParse:config_parse",
        "@libsodium//:sodium",
    ],
)