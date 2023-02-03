cc_library(
    name = "computation_party",
    srcs = [
        "ComputationParty.cpp"
    ],
    hdrs = [
        "ComputationParty.hpp",
    ],
    deps = [
        "//config_parse:config_parse",
        "//Client/ComputationToBts:client",
        "//Server/ComputationToComputationContainer:server",
        "//Server/ComputationToComputationContainerForJob:server",
        "//Server/ManageToComputationContainer:server",
        "//job:jobManager",
        "//Logging:log",
    ],
    visibility = ["//visibility:public"],
)
