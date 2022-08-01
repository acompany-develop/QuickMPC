cc_library(
    name = "gradient_descent",
    hdrs = [
        "GradientDescent.hpp",
    ],
    srcs = [
    ],
    deps = [
        "//Share:share",
        "//Optimizer:optInterface",
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
        "//Share:share",
        "//Optimizer:optInterface",
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
        "//Share:share",
        "//Optimizer:optInterface",
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
        "//Share:share",
        "//Optimizer:optInterface",
    ],
    visibility = ["//visibility:public"],
)
cc_library(
    name = "optInterface",
    hdrs = [
        "OptInterface.hpp",
    ],
    deps = [
        "//Share:share",
        "//ObjectiveFunction:objective_function",
        
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
        "//Optimizer:gradient_descent",
        "//Optimizer:sgd",
        "//Optimizer:momentum",
        "//Optimizer:adam",
    ],
    visibility = ["//visibility:public"],
)