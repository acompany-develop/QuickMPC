
cc_library(
    name = "computation_to_dbgate_test",
    hdrs = [
        "ComputationToDbGateTest.hpp",
    ],
    copts = ["-Iexternal/gtest/include"],
    deps = [
        "@gtest//:main",
        "@nlohmann_json//:json",
        "//Client/ComputationToDbGate:client",
        "//Client/AnyToDb:client",
        "//Client/AnyToDb:n1ql",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "share_comp_test",
    hdrs = [
        "ShareCompTest.hpp",
    ],
    copts = ["-Iexternal/gtest/include"],
    deps = [
        "@gtest//:main",
        "//Share:share",
        "//Share:compare",
        "//LogHeader:logger"
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "share_test",
    srcs = [
        "ShareTest.hpp",
    ],
    copts = ["-Iexternal/gtest/include"],
    deps = [
        "@gtest//:main",
        "//Random:random",
        "//Share:compare",
        "//LogHeader:logger"
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "math_test",
    srcs = [
        "MathTest.hpp",
    ],
    deps = [
        "@gtest//:main",
        "//Math:math",
        "//LogHeader:logger"
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "matrix_test",
    hdrs = [
        "MatrixTest.hpp",
    ],
    deps = [
        "@gtest//:main",
        "//Share:matrix",
        "//LogHeader:logger",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "meshcode_test",
    srcs = [
        "MeshCodeTest.hpp",
    ],
    deps = [
        "@gtest//:main",
        "//Share:share",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "model_test",
    hdrs = [
        "ModelTest.hpp",
    ],
    copts = ["-Iexternal/gtest/include"],
    deps = [
        "@gtest//:main",
        "//Share:share",
        "//Model/Models:linearregression",
        "//Model/Models:logisticregression",
        "//Model/Models:decisiontree",
        "//Job/Jobs:logisticregressionJob",
        "@Proto//ManageToComputationContainer:manage_to_computation_cc_grpc",
        "//LogHeader:logger",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "optimize_test",
    hdrs = [
        "OptimizeTest.hpp",
    ],
    deps = [
        "//ObjectiveFunction:objective_function",
        "//Optimizer:optimizer",
        "//Client/ComputationToBts:client",
        "//LogHeader:logger",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "read_triple_test",
    srcs = [
        "ReadTripleFromBtsTest.hpp",
    ],
    copts = ["-Iexternal/gtest/include"],
    deps = [
        "@gtest//:main",
        "//TripleHandler:triple_handler",
        "//LogHeader:logger",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "gbdt_test",
    hdrs = [
        "GBDTTest.hpp",
    ],
    deps = [
        "//GBDT:gbdt",
        "//Share:share",
        "//LogHeader:logger"
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "value_table_test",
    hdrs = [
        "ValueTableTest.hpp",
    ],
    deps = [
        "@gtest//:main",
        "//Client/ComputationToDbGate:valuetable",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "anytodb_gate_test",
    srcs = [
        "AnyToDbGateTest.hpp",
    ],
    copts = ["-Iexternal/gtest/include"],
    deps = [
        "@gtest//:main",
        "@nlohmann_json//:json",
        "//Client/AnyToDb:client",
        "//Client/AnyToDb:n1ql",
        "//LogHeader:logger"
    ],
    visibility = ["//visibility:public"],
)
