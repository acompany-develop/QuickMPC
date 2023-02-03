cc_library(
    name = "server",
    srcs = [
        "Server.cpp"
    ],
    hdrs = [
        "Server.hpp",
    ],
    deps = [
        "//config_parse:config_parse",
        "//Client/Helper:helper",
        "@proto//ComputationToComputationContainer:computation_to_computation_cc_grpc",
        "//Client/ComputationToComputationContainer:client",
        "//Logging:log",
        "//Server/Helper:helper",
    ],
    visibility = ["//visibility:public"],
)