cc_library(
    name = "server",
    srcs = [
        "server.cpp"
    ],
    hdrs = [
        "server.hpp",
    ],
    deps = [
        "//config_parse:config_parse",
        "//client/helper:helper",
        "@proto//computation_to_computation_container_for_job:computation_to_computation_for_job_cc_grpc",
        "//job:jobManager",
        "//logging:log",
        "//server/helper:helper",
    ],
    visibility = ["//visibility:public"],
)