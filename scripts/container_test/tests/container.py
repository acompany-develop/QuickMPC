import subprocess
from dataclasses import InitVar, dataclass
from typing import List

compose_files = [
    "core",
    "unit",
]
compose_files_opt = " ".join(
    [f"-f docker-compose.{t}.yml" for t in compose_files])


@dataclass(frozen=True)
class Container:
    service_name: str

    def up(self):
        command = f"docker-compose {compose_files_opt} up -d {self.service_name}"
        print(command)
        ret = subprocess.run([command], shell=True)
        print(ret)

    def down(self):
        command = f"docker-compose {compose_files_opt} rm -fs {self.service_name}"
        print(command)
        ret = subprocess.run([command], shell=True)
        print(ret)


@dataclass(frozen=True)
class Containers:
    service_names: List[str]

    containers: List[Container]
