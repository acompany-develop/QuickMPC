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
        "@proto//ManageToComputationContainer:manage_to_computation_cc_grpc",
        "//Share:share",
        "//Client/ComputationToDb:client",
        "//Client/ComputationToDb:valuetable",
        "//Job:jobStatus",
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
        "@proto//ManageToComputationContainer:manage_to_computation_cc_grpc",
        "//Job:jobStatus",
        "//Job:progressManager",
        "//Share:share",
        "//Model:model",
        "//Model/Models:linearregression",
        "//Model/Models:logisticregression",
        "//Model/Models:decisiontree",
        "//Model/Models:sid3",
        "//Logging:log",
        "@proto//common_types:common_types_cc_proto",
    ],
    visibility = ["//visibility:public"],
)
