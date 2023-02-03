cc_library(
    name = "client",
    srcs = [
        "client.cpp"
    ],
    hdrs = [
        "client.hpp",
    ],
    deps = [
        "@nlohmann_json//:json",
        "//client/computation_to_db:valuetable",
        "@proto//manage_to_computation_container:manage_to_computation_cc_grpc",
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
        "//share:share",
        "//share:compare",
        "//job:progressManager",
        "//logging:log"
    ],
    visibility = ["//visibility:public"],
)
