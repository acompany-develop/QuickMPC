cc_library(
    name = "log",
    hdrs = [
        "Logger.hpp",
    ],
    linkopts=["-lboost_stacktrace_addr2line"],
    defines=["BOOST_STACKTRACE_LINK",
        "BOOST_STACKTRACE_USE_ADDR2LINE"],
    deps = [
    ],
    visibility = ["//visibility:public"],
)
