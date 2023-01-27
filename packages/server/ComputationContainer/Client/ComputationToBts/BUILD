cc_library(
    name = "client",
    srcs = [
        "Client.cpp"
    ],
    hdrs = [
        "Client.hpp",
    ],
    deps = [
        "//Client/Helper:helper",
        "//ConfigParse:config_parse",
        "@proto//EngineToBts:engine_to_bts_cc_grpc",
        "//Logging:log",
    ],
    visibility = ["//visibility:public"],
)