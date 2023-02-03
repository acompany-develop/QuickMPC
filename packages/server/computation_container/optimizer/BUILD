cc_library(
    name = "gradient_descent",
    hdrs = [
        "GradientDescent.hpp",
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
        "SGD.hpp",
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
        "Momentum.hpp",
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
        "Adam.hpp",
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
        "OptInterface.hpp",
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
        "GradientDescent.hpp",
        "SGD.hpp",
        "Momentum.hpp",
        "Adam.hpp",
    ],
    deps = [
        "//optimizer:gradient_descent",
        "//optimizer:sgd",
        "//optimizer:momentum",
        "//optimizer:adam",
    ],
    visibility = ["//visibility:public"],
)