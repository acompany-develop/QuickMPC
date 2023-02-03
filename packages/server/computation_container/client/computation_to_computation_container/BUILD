cc_library(
    name = "client",
    srcs = [
        "client.cpp"
    ],
    hdrs = [
        "client.hpp",
    ],
    deps = [
        "//config_parse:config_parse",
        "//client/helper:helper",
        "@proto//computation_to_computation_container:computation_to_computation_cc_grpc",
        "//share:address",
        "//logging:log"
    ],
    visibility = ["//visibility:public"],
)
