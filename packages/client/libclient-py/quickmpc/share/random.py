import math
from abc import ABC, abstractmethod
from dataclasses import dataclass
from decimal import Decimal
from typing import ClassVar, List

from nacl.utils import random, randombytes_deterministic

from quickmpc.exception import ArgumentError
from quickmpc.utils import methoddispatch


# 乱数生成のインタフェース
class RandomInterface(ABC):
    @abstractmethod
    def get(self, a, b) -> int:
        ...

    @abstractmethod
    def get_list(self, a, b, size: int) -> List[int]:
        ...


@dataclass(frozen=True)
class ChaCha20(RandomInterface):
    def __exception_check(self, a, b) -> None:
        if a >= b:
            raise ArgumentError(
                "乱数の下限は上限より小さい必要があります．"
                f"{a} < {b}")
        if type(a) != type(b):
            raise ArgumentError(
                "乱数の下限と上限の型は一致させる必要があります．"
                f"{type(a)} != {type(b)}")

    def __get_byte_size(self, x: int) -> int:
        # 整数の byte サイズを取得
        return max(math.ceil(math.log2(x))//8 + 1, 32)

    @methoddispatch()
    def get(self, a, b) -> int:
        self.__exception_check(a, b)
        interval_byte = self.__get_byte_size(b-a)
        byte_val: bytes = random(interval_byte)
        int_val = int.from_bytes(byte_val, "big")
        return int_val % (b - a) + a

    @methoddispatch()
    def get_list(self, a, b, size: int):
        self.__exception_check(a, b)
        interval_byte = self.__get_byte_size(b-a)
        byte_list: bytes = random(interval_byte * size)
        int_list = [int.from_bytes(byte_list[i:i+interval_byte], "big")
                    for i in range(0, len(byte_list), interval_byte)]
        return [int_val % (b - a) + a for int_val in int_list]
    