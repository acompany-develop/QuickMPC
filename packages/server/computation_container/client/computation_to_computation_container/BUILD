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
        "@proto//ComputationToComputationContainer:computation_to_computation_cc_grpc",
        "//Share:address",
        "//Logging:log"
    ],
    visibility = ["//visibility:public"],
)
