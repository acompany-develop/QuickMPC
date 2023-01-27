
cc_library(
    name = "linearregression",
    hdrs = [
        "LinearRegression.hpp",
    ],
    deps = [
        "//Model:model",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "logisticregression",
    hdrs = [
        "LogisticRegression.hpp",
    ],
    deps = [
        "//Model:model",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "decisiontree",
    hdrs = [
        "DecisionTree.hpp",
    ],
    deps = [
        "//Model:model",
        "//Share:compare",
        "//Math:math",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "sid3",
    hdrs = [
        "SID3Model.hpp",
    ],
    deps = [
        "//Model:model",
        "//GBDT:sid3",
    ],
    visibility = ["//visibility:public"],
)
