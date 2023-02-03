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
        "//computation:computation",
        "//share:share",
        "@nlohmann_json//:json",
        "@proto//manage_to_computation_container:manage_to_computation_cc_grpc",
        "//server/computation_to_computation_container_for_job:server",
        "//job:jobManager",
        "@com_github_grpc_grpc//:grpc++",
        "@com_github_grpc_grpc//:grpc++_reflection",
        "//logging:log"
    ],
    visibility = ["//visibility:public"],
)
