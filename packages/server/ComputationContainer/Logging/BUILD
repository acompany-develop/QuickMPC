cc_library(
    name = "log_header",
    hdrs = [
        "LogHeader.hpp"
    ],
    deps = [
        "@libspdlog//:spdlog"
    ],
    visibility = ["//visibility:private"],
)

cc_library(
    name = "log",
    hdrs = [
        "Logger.hpp",
    ],
    linkopts=["-lboost_stacktrace_backtrace", "-ldl", "-lbacktrace"],
    defines=["BOOST_STACKTRACE_LINK",
        "BOOST_STACKTRACE_USE_BACKTRACE"],
    deps = [
        ":log_header",
    ],
    visibility = ["//visibility:public"],
)
