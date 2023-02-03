cc_library(
    name = "share",
    hdrs = [
        "share.hpp",
        "Networking.hpp",
        "AddressId.hpp"
    ],
    deps = [
        "//server/computation_to_computation_container:server",
        "//triple_handler:triple_handler",
        "//random:random"
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "compare",
    srcs = [
        "Compare.cpp"
    ],
    hdrs = [
        "Compare.hpp"
    ],
    deps = [
        "//server/computation_to_computation_container:server",
        "//triple_handler:triple_handler",
        "//random:random",
        "//share:share",
        "//logging:log"
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "address",
    hdrs = [
        "AddressId.hpp"
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "matrix",
    hdrs = [
        "Matrix.hpp",
    ],
    srcs = [
        "Matrix.cpp"
    ],
    deps = [
        "//share:share",
        "//server/computation_to_computation_container:server",
        "@eigen//:eigen",
    ],
    visibility = ["//visibility:public"],
)