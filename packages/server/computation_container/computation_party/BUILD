cc_library(
    name = "computation_party",
    srcs = [
        "computation_party.cpp"
    ],
    hdrs = [
        "computation_party.hpp",
    ],
    deps = [
        "//config_parse:config_parse",
        "//client/computation_to_bts:client",
        "//server/computation_to_computation_container:server",
        "//server/computation_to_computation_container_for_job:server",
        "//server/manage_to_computation_container:server",
        "//job:jobManager",
        "//logging:log",
    ],
    visibility = ["//visibility:public"],
)
