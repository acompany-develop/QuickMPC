cc_library(
    name = "job",
    hdrs = [
        "JobBase.hpp",
    ],
    deps = [
        "//Job:jobStatus",
        "//Computation:computation",
        "//Share:share",
        "//Client/ComputationToDb:client",
        "@Proto//ManageToComputationContainer:manage_to_computation_cc_grpc",
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
        "//Job:job",
        "//Job/Jobs:correljob",
        "//Job/Jobs:mathjob",
        "//Job/Jobs:linearregressionJob",
        "//Job/Jobs:logisticregressionJob",
        "//Job/Jobs:jointablejob",
        "//TransactionQueue:transaction_queue",
        "//Job/Jobs:meshcodeJob",
        "//Client/ComputationToComputationContainerForJob:client",
        "//ConfigParse:config_parse",
        "//Job/Jobs:sid3Job",
        "//Job:progressManager",
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
        "@Proto//common_types:common_types_cc_proto",
        "//Logging:log",
        "//Job:progressManager",
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
        "@Proto//common_types:common_types_cc_proto",
    ],
    visibility = ["//visibility:public"],
)
