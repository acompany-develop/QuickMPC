cc_library(
    name = "objective_function",
    hdrs = [
        "ObjectiveFunctionInterface.hpp",
    ],
    deps = [
        "//Share:share",
    ],
    visibility = ["//visibility:public"],
)
cc_library(
    name = "cross_entropy",
    hdrs = [
        "CrossEntropyError.hpp",
    ],
    deps = [
        "//ObjectiveFunction:objective_function",
        "//Logging:log",
    ],
    visibility = ["//visibility:public"],
)