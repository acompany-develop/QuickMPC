import glob

import autopep8
import pkg_resources
from grpc.tools import protoc

_output_path: str = "../packages/client/libclient-py/quickmpc/proto"


def generate_files():
    protoc.main(
        (
            "",
            "-I{}".format(pkg_resources.resource_filename('grpc_tools',
                                                          '_proto')),
            "-I./libc_to_manage_container",
            "-I./libc_to_manage_container_v0",
            "-I./",
            f"--python_out={_output_path}",
            f"--grpc_python_out={_output_path}",
            f"--mypy_out={_output_path}",
            f"--mypy_grpc_out={_output_path}",
            "./libc_to_manage_container/libc_to_manage.proto",
            "./libc_to_manage_container_v0/libc_to_manage_v0.proto",
        )
    )
    protoc.main(
        (
            "",
            "-I./",
            f"--python_out={_output_path}",
            f"--mypy_out={_output_path}",
            "./common_types/common_types.proto",
        )
    )


def format_files():
    files = glob.glob(f"{_output_path}/**/*.py", recursive=True)
    for file in files:
        autopep8.main(["autopep8", "--in-place", file])


if __name__ == '__main__':
    print(f"path: `./`")
    generate_files()
    print("generate finished")
    format_files()
    print("format finished")
