cc_library(
    name = "client",
    srcs = [
        "Client.cpp"
    ],
    hdrs = [
        "Client.hpp",
    ],
    deps = [
        "//config_parse:config_parse",
        "//Client/Helper:helper",
        "@proto//ComputationToComputationContainerForJob:computation_to_computation_for_job_cc_grpc",
        "//Share:address",
        "//Logging:log",
    ],
    visibility = ["//visibility:public"],
)