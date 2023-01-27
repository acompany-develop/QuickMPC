import glob

import autopep8
import pkg_resources
from grpc.tools import protoc


def generate_files():
    protoc.main(
        (
            "",
            "-I{}".format(pkg_resources.resource_filename('grpc_tools',
                                                          '_proto')),
            "-I.",
            "--python_out=.",
            "--grpc_python_out=.",
            "./libc_to_manage.proto",
        )
    )
    protoc.main(
        (
            "",
            "-I.",
            "--python_out=.",
            "./common_types/common_types.proto",
        )
    )


def format_files():
    files = glob.glob("./**/*.py", recursive=True)
    for file in files:
        autopep8.main(["autopep8", "--in-place", file])


if __name__ == '__main__':
    generate_files()
    print("generate finished")
    format_files()
    print("format finished")
