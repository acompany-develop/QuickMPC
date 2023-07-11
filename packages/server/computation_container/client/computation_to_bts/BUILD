cc_library(
    name = "client",
    srcs = [
        "client.cpp"
    ],
    hdrs = [
        "client.hpp",
    ],
    deps = [
        "//client/helper:helper",
        "//share:address",
        "//config_parse:config_parse",
        "@proto//engine_to_bts:engine_to_bts_cc_grpc",
        "//logging:log",
    ],
    visibility = ["//visibility:public"],
)