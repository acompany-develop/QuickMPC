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
        "//Computation:computation",
        "//Share:share",
        "@nlohmann_json//:json",
        "@proto//ManageToComputationContainer:manage_to_computation_cc_grpc",
        "//Server/ComputationToComputationContainerForJob:server",
        "//job:jobManager",
        "@com_github_grpc_grpc//:grpc++",
        "@com_github_grpc_grpc//:grpc++_reflection",
        "//Logging:log"
    ],
    visibility = ["//visibility:public"],
)
