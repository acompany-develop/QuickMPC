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
        "@proto//ManageToComputationContainer:manage_to_computation_cc_grpc",
        "@proto//common_types:common_types_cc_proto",
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
        "//Logging:log"
    ],
    visibility = ["//visibility:public"],
)
