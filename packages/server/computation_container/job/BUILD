cc_library(
    name = "job",
    hdrs = [
        "JobBase.hpp",
    ],
    deps = [
        "//job:jobStatus",
        "//computation:computation",
        "//share:share",
        "//client/computation_to_db:client",
        "@proto//manage_to_computation_container:manage_to_computation_cc_grpc",
        "//logging:log"
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
        "//transaction_queue:transaction_queue",
        "//job/jobs:meshcodeJob",
        "//client/computation_to_computation_container_for_job:client",
        "//client/computation_to_bts:client",
        "//config_parse:config_parse",
        "//job:progressManager",
        "//logging:log"
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "jobStatus",
    hdrs = [
        "JobStatus.hpp",
    ],
    deps = [
        "//client/computation_to_db:client",
        "@proto//common_types:common_types_cc_proto",
        "//logging:log",
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
        "//logging:log",
        "@proto//common_types:common_types_cc_proto",
    ],
    visibility = ["//visibility:public"],
)
