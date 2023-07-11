cc_library(
    name = "bts_handler",
    srcs = [
        "stock_bts.cpp"
    ],
    hdrs = [
        "stock_bts.hpp"
    ],
    deps = [
        "//client/computation_to_bts:client",
        "//bts_handler:job"
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "job",
    hdrs = [
        "job.hpp",
    ],
    visibility = ["//visibility:public"],
)