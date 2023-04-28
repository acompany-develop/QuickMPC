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
        "//fixed_point:fixed_point"
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "csprng",
    srcs = [
        "csprng.cpp"
    ],
    hdrs = [
        "csprng.hpp",
        "csprng_interface.hpp"
    ],
    deps = [
        "@libsodium//:sodium",
        "//logging:log",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "csprng_interface",
    hdrs = [
        "csprng_interface.hpp",
    ],
    deps = [
        "@libsodium//:sodium",
        "//logging:log",
    ],
    visibility = ["//visibility:public"],
)