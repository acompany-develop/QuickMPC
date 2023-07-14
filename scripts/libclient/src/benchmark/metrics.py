import dataclasses
import time

import pytest
from tabulate import tabulate


@dataclasses.dataclass
class Timer:
    __start: float = dataclasses.field(init=False)
    __elapsed_time: float = dataclasses.field(init=False)

    def __enter__(self):
        self.__start = time.time()
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        self.__elapsed_time = time.time() - self.__start

    def elapsed_time(self):
        return self.__elapsed_time


@dataclasses.dataclass
class ElapsedTimeDict:
    __elapsed_time_dict: dict = dataclasses.field(default_factory=dict)

    def save_elapsed_time(self, elapsed_time: float, name: str, path: str):
        if (path, name) in self.__elapsed_time_dict:
            self.__elapsed_time_dict[(path, name)] = max(
                self.__elapsed_time_dict[(path, name)], elapsed_time)
        else:
            self.__elapsed_time_dict[(path, name)] = elapsed_time

    def print(self):
        headers = ["path", "job", "elapsed_time[s]", "elapsed_time[m]"]
        data = []
        for key, elapsed_time_s in self.__elapsed_time_dict.items():
            elapsed_time_m = elapsed_time_s / 60
            data.append([*key, elapsed_time_s, elapsed_time_m])
        data.sort()
        # NOTE: CIでのbenchmarkの際に、`Start of Table`から`End of Table`までがslackに送信される
        print("\n============================= Start of Table =============================")
        print(tabulate(data, headers=headers, tablefmt='fancy_grid'))
        print("============================= End of Table =============================")


@pytest.fixture(scope='module')
def print_elapsed_time():
    et_dict = ElapsedTimeDict()
    # テスト内で計測結果を記録
    yield et_dict.save_elapsed_time
    # 計測結果出力
    et_dict.print()
