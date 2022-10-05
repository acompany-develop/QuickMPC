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
    ],
    visibility = ["//visibility:public"],
)

