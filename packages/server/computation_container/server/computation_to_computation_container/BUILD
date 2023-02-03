cc_library(
    name = "server",
    srcs = [
        "server.cpp"
    ],
    hdrs = [
        "server.hpp",
    ],
    deps = [
        "//config_parse:config_parse",
        "//client/helper:helper",
        "@proto//computation_to_computation_container:computation_to_computation_cc_grpc",
        "//client/computation_to_computation_container:client",
        "//logging:log",
        "//server/helper:helper",
    ],
    visibility = ["//visibility:public"],
)