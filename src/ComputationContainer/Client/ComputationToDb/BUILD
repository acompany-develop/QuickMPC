cc_library(
    name = "client",
    srcs = [
        "Client.cpp"
    ],
    hdrs = [
        "Client.hpp",
    ],
    deps = [
        "@nlohmann_json//:json",
        "//Client/ComputationToDbGate:valuetable",
        "@Proto//ManageToComputationContainer:manage_to_computation_cc_grpc",
        "@Proto//common_types:common_types_cc_proto",
    ],
    linkopts = [
        "-lstdc++fs"
    ],
    visibility = ["//visibility:public"],
)

