cc_library(
    name = "client",
    srcs = [
        "Client.cpp"
    ],
    hdrs = [
        "Client.hpp",
    ],
    deps = [
        "//ConfigParse:config_parse",
        "//Client/Helper:helper",
        "@Proto//ComputationToComputationContainer:computation_to_computation_cc_grpc",
        "//Share:address",
        "//LogHeader:logger"
    ],
    visibility = ["//visibility:public"],
)
