cc_library(
    name = "model",
    hdrs = [
        "ModelBase.hpp",
    ],
    srcs = [
        "ModelBase.cpp"
    ],
    deps = [
        "@nlohmann_json//:json",
        "@Proto//ManageToComputationContainer:manage_to_computation_cc_grpc",
        "//Share:share",
        "//Client/ComputationToDb:client",
        "//Client/ComputationToDb:valuetable",
        "//Job:jobStatus",
        "//Job:progressManager",
        "//Logging:log",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "modelManager",
    hdrs = [
        "ModelManager.hpp",
    ],
    deps = [
        "@Proto//ManageToComputationContainer:manage_to_computation_cc_grpc",
        "//Model:model",
        "//Model/Models:linearregression",
        "//Model/Models:logisticregression",
        "//Model/Models:decisiontree",
        "//Model/Models:sid3",
        "//Logging:log",
        "@Proto//common_types:common_types_cc_proto",
    ],
    visibility = ["//visibility:public"],
)
