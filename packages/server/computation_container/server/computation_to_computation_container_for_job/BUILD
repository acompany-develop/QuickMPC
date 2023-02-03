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
        "@proto//ComputationToComputationContainerForJob:computation_to_computation_for_job_cc_grpc",
        "//job:jobManager",
        "//Logging:log",
        "//Server/Helper:helper",
    ],
    visibility = ["//visibility:public"],
)