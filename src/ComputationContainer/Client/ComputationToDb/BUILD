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
        "//Client/ComputationToDb:valuetable",
        "@Proto//ManageToComputationContainer:manage_to_computation_cc_grpc",
        "@Proto//common_types:common_types_cc_proto",
    ],
    linkopts = [
        "-lstdc++fs"
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "valuetable",
    srcs = [
        "ValueTable.cpp"
    ],
    hdrs = [
        "ValueTable.hpp",
    ],
    deps = [
        "@nlohmann_json//:json",
        "//Share:share",
        "//Share:compare",
        "//Job:progressManager",
    ],
    visibility = ["//visibility:public"],
)
