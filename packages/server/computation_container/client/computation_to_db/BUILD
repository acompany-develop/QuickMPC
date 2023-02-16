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
        "@proto//common_types:common_types_cc_proto",
        "//logging:log",
    ],
    linkopts = [
        "-lstdc++fs"
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "jointable",
    srcs = [
        "join_table.cpp"
    ],
    hdrs = [
        "join_table.hpp",
    ],
    deps = [
        "@nlohmann_json//:json",
        "@proto//manage_to_computation_container:manage_to_computation_cc_grpc",
        "//share:share",
        "//share:compare",
        "//job:progressManager",
        "//logging:log",
        "//client/computation_to_db:client",
        "//client/computation_to_db:valuetable",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "valuetable",
    srcs = [
        "value_table.cpp"
    ],
    hdrs = [
        "value_table.hpp",
    ],
    deps = [
        "//logging:log",
        "//client/computation_to_db:client",
    ],
    visibility = ["//visibility:public"],
)
