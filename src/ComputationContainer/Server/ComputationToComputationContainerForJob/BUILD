cc_library(
    name = "server",
    srcs = [
        "Server.cpp"
    ],
    hdrs = [
        "Server.hpp",
    ],
    deps = [
        "//ConfigParse:config_parse",
        "//Client/Helper:helper",
        "@Proto//ComputationToComputationContainerForJob:computation_to_computation_for_job_cc_grpc",
        "//Job:jobManager",
        "//LogHeader:logger",
        "//Server/Helper:helper",
    ],
    visibility = ["//visibility:public"],
)