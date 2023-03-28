import json
import subprocess
import time
from concurrent.futures import ProcessPoolExecutor
from dataclasses import InitVar, dataclass, field
from typing import ClassVar, List

compose_files = [
    "core",
    "unit",
    "medium",
    "dev",
]
compose_files_opt = " ".join(
    [f"-f docker-compose.{t}.yml" for t in compose_files])


@dataclass(frozen=True)
class Container:
    __service_name: str

    __healthcheck_interval: ClassVar[int] = 3
    __healthcheck_num: ClassVar[int] = 10

    def __container_id(self) -> str:
        command = f"docker-compose {compose_files_opt} " + \
            f"ps -q {self.__service_name}"
        container_id = subprocess.check_output([command], shell=True)
        return container_id.decode("utf-8")

    def __healthcheck(self) -> bool:
        command = "docker inspect --format ='{{json .State.Health}}' " + \
            f"{self.__container_id()}"
        row = subprocess.check_output([command], shell=True)
        json_log = json.loads(row.decode("utf-8")[1:])
        return json_log["Status"] == "healthy"

    def up(self) -> None:
        command = f"docker-compose {compose_files_opt} " + \
            f"up -d {self.__service_name}"
        subprocess.run([command], shell=True)
        # healthcheckが通るまで待機，timeoutを過ぎたらraise
        for _ in range(self.__healthcheck_num):
            if self.__healthcheck():
                return
            time.sleep(self.__healthcheck_interval)
        self.down()
        raise RuntimeError(f"`{self.__service_name}` is not healthy.")

    def down(self) -> None:
        cid = self.__container_id()
        if not cid:
            # コンテナが立ってなかったらdownしない
            return
        command = f"docker-compose {compose_files_opt} " + \
            f"rm -fs {self.__service_name}"
        subprocess.run([command], shell=True)


@dataclass(frozen=True)
class Containers:
    service_names: InitVar[List[str]]

    __containers: List[Container] = field(init=False)

    def __post_init__(self, service_names: List[str]) -> None:
        containers = [Container(name) for name in service_names]
        object.__setattr__(self, "_Containers__containers", containers)

    def up(self) -> None:
        with ProcessPoolExecutor() as executor:
            for c in self.__containers:
                executor.submit(c.up)

    def down(self) -> None:
        with ProcessPoolExecutor() as executor:
            for c in self.__containers:
                executor.submit(c.down)
