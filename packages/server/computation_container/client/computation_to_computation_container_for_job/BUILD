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
        "@proto//computation_to_computation_container_for_job:computation_to_computation_for_job_cc_grpc",
        "//share:address",
        "//logging:log",
    ],
    visibility = ["//visibility:public"],
)