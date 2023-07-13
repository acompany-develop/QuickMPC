cc_library(
    name = "share",
    hdrs = [
        "share.hpp",
        "networking.hpp",
        "address_id.hpp"
    ],
    deps = [
        "//server/computation_to_computation_container:server",
        "//bts_handler:bts_handler",
        "//random:random",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "compare",
    srcs = [
        "compare.cpp"
    ],
    hdrs = [
        "compare.hpp"
    ],
    deps = [
        "//server/computation_to_computation_container:server",
        "//random:random",
        "//share:share",
        "//logging:log",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "address",
    hdrs = [
        "address_id.hpp"
    ],
    visibility = ["//visibility:public"],
)
