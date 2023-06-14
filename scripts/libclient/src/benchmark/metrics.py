import dataclasses
import time


@dataclasses.dataclass
class PrintTime:

    __name: str
    __start: float = dataclasses.field(init=False)

    def __enter__(self):
        self.__start = time.time()
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        elapsed_time = time.time() - self.__start
        print(f"{self.__name}: {elapsed_time} ms")
