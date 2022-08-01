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
        "@Proto//EngineToBts:engine_to_bts_cc_grpc",
        "//LogHeader:logger",
        "//Logging:log",
    ],
    visibility = ["//visibility:public"],
)