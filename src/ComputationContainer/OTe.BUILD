licenses(["notice"])  # ISC (簡略化されたBSDおよびMITライセンスと機能的に同等)

exports_files(["LICENSE.ISC"])

cc_library(
    name = "OTe",
    srcs = [
        "lib/liblibOTe_Tests.a",
        "lib/liblibOTe.a",
        "lib/libcryptoTools.a",
        "lib/libcoproto.a",
        "lib/libmacoro.a",],
    hdrs = glob(["include/libOTe/*.h","include/liblibOTe_Tests/*.h"]),
    linkstatic=1,
    linkopts = ["-I/usr/lcoal/include"],
    visibility = ["//visibility:public"],
)