cc_library(
    name = "computation_party",
    srcs = [
        "ComputationParty.cpp"
    ],
    hdrs = [
        "ComputationParty.hpp",
    ],
    deps = [
        "//ConfigParse:config_parse",
        "//Server/ComputationToComputationContainer:server",
        "//Server/ComputationToComputationContainerForJob:server",
        "//Server/ManageToComputationContainer:server",
        "//Job:jobManager",
        "//Logging:log",
    ],
    visibility = ["//visibility:public"],
)
