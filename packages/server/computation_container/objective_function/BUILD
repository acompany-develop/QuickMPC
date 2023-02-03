cc_library(
    name = "objective_function",
    hdrs = [
        "objective_function_interface.hpp",
    ],
    deps = [
        "//share:share",
    ],
    visibility = ["//visibility:public"],
)
cc_library(
    name = "cross_entropy",
    hdrs = [
        "cross_entropy_error.hpp",
    ],
    deps = [
        "//objective_function:objective_function",
        "//logging:log",
    ],
    visibility = ["//visibility:public"],
)