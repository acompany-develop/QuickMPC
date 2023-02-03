cc_library(
    name = "job",
    hdrs = [
        "JobBase.hpp",
    ],
    deps = [
        "//job:jobStatus",
        "//Computation:computation",
        "//Share:share",
        "//Client/ComputationToDb:client",
        "@proto//ManageToComputationContainer:manage_to_computation_cc_grpc",
        "//Logging:log"
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "jobManager",
    hdrs = [
        "JobManager.hpp",
        "JobSelector.hpp",
        "JobParameter.hpp"
    ],
    deps = [
        "//job:job",
        "//job/jobs:correljob",
        "//job/jobs:mathjob",
        "//job/jobs:jointablejob",
        "//TransactionQueue:transaction_queue",
        "//job/jobs:meshcodeJob",
        "//Client/ComputationToComputationContainerForJob:client",
        "//Client/ComputationToBts:client",
        "//config_parse:config_parse",
        "//job:progressManager",
        "//Logging:log"
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "jobStatus",
    hdrs = [
        "JobStatus.hpp",
    ],
    deps = [
        "//Client/ComputationToDb:client",
        "@proto//common_types:common_types_cc_proto",
        "//Logging:log",
        "//job:progressManager",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "progressManager",
    srcs = [
        "ProgressManager.cpp",
    ],
    hdrs = [
        "ProgressManager.hpp",
    ],
    deps = [
        "//Logging:log",
        "@proto//common_types:common_types_cc_proto",
    ],
    visibility = ["//visibility:public"],
)
