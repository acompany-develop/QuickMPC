cc_library(
    name = "share",
    hdrs = [
        "Share.hpp",
        "Networking.hpp",
        "AddressId.hpp"
    ],
    deps = [
        "//Server/ComputationToComputationContainer:server",
        "//TripleHandler:triple_handler",
        "//Random:random"
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
        "//Server/ComputationToComputationContainer:server",
        "//TripleHandler:triple_handler",
        "//Random:random",
        "//Share:share",
        "//LogHeader:logger"
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
        "//Share:share",
        "//Server/ComputationToComputationContainer:server",
        "@eigen//:eigen",
    ],
    visibility = ["//visibility:public"],
)