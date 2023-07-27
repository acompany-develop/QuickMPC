cc_library(
    name = "bts_handler",
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
    deps = [
        "//fixed_point:fixed_point"
    ]
    visibility = ["//visibility:public"],
)