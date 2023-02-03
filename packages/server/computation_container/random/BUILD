cc_library(
    name = "random",
    srcs = [
        "random.cpp"
    ],
    hdrs = [
        "random.hpp",
    ],
    deps = [
        "//random:csprng",
        "@libsodium//:sodium",
        "//FixedPoint:fixed_point"
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "csprng",
    srcs = [
        "Csprng.cpp"
    ],
    hdrs = [
        "Csprng.hpp",
    ],
    deps = [
        "@libsodium//:sodium",
        "//Logging:log",
    ],
    visibility = ["//visibility:public"],
)
