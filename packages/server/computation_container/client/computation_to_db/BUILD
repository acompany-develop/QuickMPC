cc_library(
    name = "client",
    srcs = [
        "client.cpp"
    ],
    hdrs = [
        "client.hpp",
    ],
    deps = [
        "@nlohmann_json//:json",
        "@proto//common_types:common_types_cc_proto",
        "//logging:log",
    ],
    linkopts = [
        "-lstdc++fs"
    ],
    visibility = ["//visibility:public"],
)
