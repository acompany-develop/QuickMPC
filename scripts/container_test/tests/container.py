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
class Containers:
    __service_names: List[str]

    __healthcheck_interval: ClassVar[int] = 3
    __healthcheck_num: ClassVar[int] = 10

    @staticmethod
    def __container_id(service_name: str) -> str:
        command = f"docker-compose {compose_files_opt} ps -q {service_name}"
        container_id = subprocess.check_output([command], shell=True)
        return container_id.decode("utf-8")[:-1]

    @staticmethod
    def __healthcheck(service_name: str) -> bool:
        cid: str = Containers.__container_id(service_name)
        command = "docker inspect --format ='{{json .State.Health}}' " f"{cid}"
        for _ in range(Containers.__healthcheck_num):
            row = subprocess.check_output([command], shell=True)
            json_log = json.loads(row.decode("utf-8")[1:])
            # NOTE: health checkが導入されてないコンテナもTrueとする
            if not json_log or json_log["Status"] == "healthy":
                return True
            time.sleep(Containers.__healthcheck_interval)
        return False

    def up(self) -> None:
        sn_str = " ".join(self.__service_names)
        command = f"docker-compose {compose_files_opt} up -d {sn_str}"
        subprocess.run([command], shell=True)
        # healthcheckが通るまで待機，timeoutを過ぎたらraise
        for sn in self.__service_names:
            if not Containers.__healthcheck(sn):
                self.down()
                raise RuntimeError(f"`{sn}` is not healthy.")

    def down(self) -> None:
        sn_str = " ".join(self.__service_names)
        command = f"docker-compose {compose_files_opt} rm -fs {sn_str}"
        subprocess.run([command], shell=True)

    def restart(self) -> None:
        sn_str = " ".join(self.__service_names)
        command = f"docker-compose {compose_files_opt} restart {sn_str}"
        subprocess.run([command], shell=True)
        # healthcheckが通るまで待機，timeoutを過ぎたらraise
        for sn in self.__service_names:
            if not Containers.__healthcheck(sn):
                self.down()
                raise RuntimeError(f"`{sn}` is not healthy.")
