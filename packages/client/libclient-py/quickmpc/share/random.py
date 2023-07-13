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

    # 128bit符号付き整数最大，最小値
    mx: ClassVar[int] = (1 << 128)-1
    mn: ClassVar[int] = -(1 << 128)

    @methoddispatch()
    def get(self, a, b):
        raise ArgumentError(
            "乱数の閾値はどちらもintもしくはdecimalでなければなりません．"
            f"a is {type(a)}, b is {type(b)}")

    @get.register(int)
    def __get_int(self, a: int, b: int) -> int:
        # TRNGで [a,b) の乱数生成
        self.__exception_check(a, b)
        interval_byte = self.__get_byte_size(b-a)
        byte_val: bytes = random(interval_byte)
        int_val = int.from_bytes(byte_val, "big")
        return int_val % (b - a) + a

    @get.register(Decimal)
    def __get_decimal(self, a: Decimal, b: Decimal) -> Decimal:
        # 256bit整数を取り出して[a,b]に正規化する
        self.__exception_check(a, b)
        val: int = self.get(self.mn, self.mx)
        return Decimal(val-self.mn)/(self.mx-self.mn)*(b-a)+a

    @methoddispatch()
    def get_list(self, a, b, size: int):
        raise ArgumentError(
            "乱数の閾値はどちらもintもしくはdecimalでなければなりません．"
            f"a is {type(a)}, b is {type(b)}")

    @get_list.register(int)
    def __get_list_int(self, a: int, b: int, size: int) -> List[int]:
        # TRNGの32byteをseedとしてCSPRNGでsize分生成
        byte_size: int = self.__get_byte_size(b-a)
        self.__exception_check(a, b)
        seed: bytes = self.__get_32byte()
        bytes_list: bytes = randombytes_deterministic(size*byte_size, seed)
        int_list = [int.from_bytes(bytes_list[i:i+byte_size], "big")
                    for i in range(0, len(bytes_list), byte_size)]
        return [x % (b-a)+a for x in int_list]

    @get_list.register(Decimal)
    def __get_list_decimal(self, a: Decimal, b: Decimal, size: int) \
            -> List[Decimal]:
        # 128bit整数を取り出して[a,b]に正規化する
        self.__exception_check(a, b)
        valList: List[int] = self.get_list(self.mn, self.mx, size)
        return [Decimal(val-self.mn)/(self.mx-self.mn)*(b-a)+a
                for val in valList]

    def __get_byte_size(self, x: int) -> int:
        # 整数の byte サイズを取得
        return max(math.ceil(math.log2(x))//8 + 1, 32)

    def __get_32byte(self) -> bytes:
        return random()

    def __exception_check(self, a, b) -> None:
        if a >= b:
            raise ArgumentError(
                "乱数の下限は上限より小さい必要があります．"
                f"{a} < {b}")
        if type(a) != type(b):
            raise ArgumentError(
                "乱数の下限と上限の型は一致させる必要があります．"
                f"{type(a)} != {type(b)}")
