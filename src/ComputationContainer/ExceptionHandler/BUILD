cc_binary(
    name = "exception_handler",
    srcs = [
        "ExceptionHandler.cpp",
    ],
    linkopts=[
        "-lboost_stacktrace_backtrace",
        "-ldl",
        "-lbacktrace",
    ],
    defines=[
        "BOOST_STACKTRACE_LINK",
        "BOOST_STACKTRACE_USE_BACKTRACE"
    ],
    linkshared=True,
    visibility = ["//visibility:public"],
)
