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
        "//Client/AnyToDb:client",
        "//Client/AnyToDb:n1ql",
        "//Client/ComputationToDbGate:valuetable",
        "@Proto//ManageToComputationContainer:manage_to_computation_cc_grpc",
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
    ],
    visibility = ["//visibility:public"],
)