import glob

import autopep8
import pkg_resources
from grpc.tools import protoc

_proto_path: str = "./../../../../../proto"


def generate_files():
    protoc.main(
        (
            "",
            "-I{}".format(pkg_resources.resource_filename('grpc_tools',
                                                          '_proto')),
            f"-I{_proto_path}/LibcToManageContainer",
            f"-I{_proto_path}",
            "--python_out=.",
            "--grpc_python_out=.",
            f"{_proto_path}/LibcToManageContainer/libc_to_manage.proto",
        )
    )
    protoc.main(
        (
            "",
            f"-I{_proto_path}",
            "--python_out=.",
            f"{_proto_path}/common_types/common_types.proto",
        )
    )


def format_files():
    files = glob.glob("./**/*.py", recursive=True)
    for file in files:
        autopep8.main(["autopep8", "--in-place", file])


if __name__ == '__main__':
    print(f"path: `{_proto_path}`")
    generate_files()
    print("generate finished")
    format_files()
    print("format finished")
