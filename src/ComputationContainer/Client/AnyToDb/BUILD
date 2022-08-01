cc_library(
    name = "client",
    srcs = [
        "Client.cpp"
    ],
    hdrs = [
        "Client.hpp"
    ],
    deps = [
        "@Proto//AnyToDbGate:any_to_db_gate_cc_grpc",
        "//ConfigParse:config_parse",
        "//LogHeader:logger",
        "//Logging:log",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "n1ql",
    srcs = [
        "N1QL.cpp"
    ],
    hdrs = [
        "N1QL.hpp"
    ],
    deps = [
        "@com_github_grpc_grpc//:grpc++",
        "//LogHeader:logger"
    ],
    visibility = ["//visibility:public"],
)
