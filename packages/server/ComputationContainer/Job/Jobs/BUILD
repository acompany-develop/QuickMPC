
cc_library(
    name = "correljob",
    hdrs = [
        "CorrelJob.hpp"
    ],
    deps = [
        "//Job:job",
    ],
    visibility = ["//visibility:public"],
)


cc_library(
    name = "mathjob",
    hdrs = [
        "MathJob.hpp"
    ],
    deps = [
        "//Job:job",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "linearregressionJob",
    hdrs = [
        "LinearRegressionJob.hpp"
    ],
    deps = [
        "//Job:job",
        "//Share:matrix",
        "//ConfigParse:config_parse",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "logisticregressionJob",
    hdrs = [
        "LogisticRegressionJob.hpp"
    ],
    deps = [
        "//Job:job",
        "//Share:matrix",
        "//Optimizer:optimizer",
        "//ObjectiveFunction:cross_entropy",
        "//Logging:log",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "meshcodeJob",
    hdrs = [
        "MeshCodeJob.hpp"
    ],
    deps = [
        "//Job:job"
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "sid3Job",
    hdrs = [
        "SID3Job.hpp"
    ],
    deps = [
        "//Job:job",
        "//GBDT:gbdt",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "jointablejob",
    hdrs = [
        "JoinTableJob.hpp"
    ],
    deps = [
        "//Job:job",
    ],
    visibility = ["//visibility:public"],
)
