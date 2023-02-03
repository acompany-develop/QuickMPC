cc_library(
    name = "gradient_descent",
    hdrs = [
        "gradient_descent.hpp",
    ],
    srcs = [
    ],
    deps = [
        "//share:share",
        "//optimizer:optInterface",
    ],
    visibility = ["//visibility:public"],
)
cc_library(
    name = "sgd",
    hdrs = [
        "sgd.hpp",
    ],
    srcs = [
    ],
    deps = [
        "//share:share",
        "//optimizer:optInterface",
    ],
    visibility = ["//visibility:public"],
)
cc_library(
    name = "momentum",
    hdrs = [
        "momentum.hpp",
    ],
    srcs = [
    ],
    deps = [
        "//share:share",
        "//optimizer:optInterface",
    ],
    visibility = ["//visibility:public"],
)
cc_library(
    name = "adam",
    hdrs = [
        "adam.hpp",
    ],
    srcs = [
    ],
    deps = [
        "//share:share",
        "//optimizer:optInterface",
    ],
    visibility = ["//visibility:public"],
)
cc_library(
    name = "optInterface",
    hdrs = [
        "opt_interface.hpp",
    ],
    deps = [
        "//share:share",
        "//objective_function:objective_function",

    ],
    visibility = ["//visibility:public"],
)
cc_library(
    name = "optimizer",
    hdrs = [
        "optimizer.hpp",
        "gradient_descent.hpp",
        "sgd.hpp",
        "momentum.hpp",
        "adam.hpp",
    ],
    deps = [
        "//optimizer:gradient_descent",
        "//optimizer:sgd",
        "//optimizer:momentum",
        "//optimizer:adam",
    ],
    visibility = ["//visibility:public"],
)