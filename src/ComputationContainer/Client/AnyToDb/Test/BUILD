cc_test(
    name = "AnyToDbGateTest",
    srcs = [
        "AnyToDbGateTest.cpp",
    ],
    copts = ["-Iexternal/gtest/include"],
    deps = [
        "@gtest//:main",
        "@nlohmann_json//:json",
        "//Client/AnyToDb:client",
        "//Client/AnyToDb:n1ql",
        "//LogHeader:logger"
    ],
    visibility = ["//visibility:public"],
)
