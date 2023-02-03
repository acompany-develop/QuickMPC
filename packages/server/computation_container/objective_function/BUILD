cc_library(
    name = "objective_function",
    hdrs = [
        "ObjectiveFunctionInterface.hpp",
    ],
    deps = [
        "//share:share",
    ],
    visibility = ["//visibility:public"],
)
cc_library(
    name = "cross_entropy",
    hdrs = [
        "CrossEntropyError.hpp",
    ],
    deps = [
        "//objective_function:objective_function",
        "//logging:log",
    ],
    visibility = ["//visibility:public"],
)