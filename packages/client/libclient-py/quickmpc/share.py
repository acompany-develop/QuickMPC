import logging
from dataclasses import dataclass
from decimal import Decimal
from typing import (Any, Callable, ClassVar, List,
                    Optional, Sequence, Tuple, Union)

import numpy as np

from .exception import ArgumentError
from .proto.common_types.common_types_pb2 import Schema, ShareValueTypeEnum
from .utils.overload_tools import (DictList, DictList2, Dim1,
                                   Dim2, Dim3, methoddispatch)
from .utils.random import ChaCha20, RandomInterface

logger = logging.getLogger(__name__)


@dataclass(frozen=True)
class Share:
    __share_random_range: ClassVar[Tuple[Decimal, Decimal]] =\
        (Decimal(-(1 << 64)), Decimal(1 << 64))

    @methoddispatch(is_static_method=True)
    @staticmethod
    def __to_str(_):
        logger.error("Invalid argument on stringfy.")
        raise ArgumentError("不正な引数が与えられています．")

    @__to_str.register(Decimal)
    @staticmethod
    def __decimal_to_str(val: Decimal) -> str:
        # InfinityをCCで読み込めるinfに変換
        return 'inf' if Decimal.is_infinite(val) else str(val)

    @__to_str.register(int)
    @staticmethod
    def __int_to_str(val: int) -> str:
        return str(val)

    @methoddispatch(is_static_method=True)
    @staticmethod
    def sharize(_, __):
        logger.error("Invalid argument on sharize.")
        raise ArgumentError("不正な引数が与えられています．")

    @methoddispatch(is_static_method=True)
    @staticmethod
    def recons(_):
        logger.error("Invalid argument on recons.")
        raise ArgumentError("不正な引数が与えられています．")

    @methoddispatch(is_static_method=True)
    @staticmethod
    def convert_type(_, __):
        logger.error("Invalid argument on convert_type.")
        raise ArgumentError("不正な引数が与えられています．")

    @sharize.register(int)
    @sharize.register(float)
    @staticmethod
    def __sharize_scalar(secrets: float, party_size: int = 3) -> List[str]:
        """ スカラ値のシェア化 """
        rnd: RandomInterface = ChaCha20()
        shares: List[int] = rnd.get_list(
            *Share.__share_random_range, party_size)
        shares[0] += Decimal(secrets) - np.sum(shares)
        shares_str: List[str] = [str(n) for n in shares]
        return shares_str

    @sharize.register((Dim1, float))
    @staticmethod
    def __sharize_1dimension_float(secrets: List[Union[float, Decimal]],
                                   party_size: int = 3) \
            -> List[List[str]]:
        """ 1次元リストのシェア化 """
        rnd: RandomInterface = ChaCha20()
        secrets_size: int = len(secrets)
        shares: np.ndarray = np.array([
            rnd.get_list(*Share.__share_random_range, secrets_size)
            for __ in range(party_size - 1)])
        s1: np.ndarray = np.subtract(np.frompyfunc(Decimal, 1, 1)(secrets),
                                     np.sum(shares, axis=0))
        shares_str: List[List[str]] = \
            np.vectorize(Share.__to_str)([s1, *shares]).tolist()
        return shares_str

    @sharize.register((Dim1, Decimal))
    @staticmethod
    def __sharize_1dimension_decimal(secrets: List[Decimal],
                                     party_size: int = 3) \
            -> List[List[str]]:
        return Share.__sharize_1dimension_float(secrets, party_size)

    @sharize.register((Dim1, int))
    @staticmethod
    def __sharize_1dimension_int(secrets: List[int], party_size: int = 3) \
            -> List[List[str]]:
        """ 1次元リストのシェア化 """
        rnd: RandomInterface = ChaCha20()
        secrets_size: int = len(secrets)
        max_val = (max(secrets)+1) * 2
        shares: np.ndarray = np.array([
            rnd.get_list(-max_val, max_val, secrets_size)
            for __ in range(party_size - 1)])
        s1: np.ndarray = np.subtract(np.frompyfunc(int, 1, 1)(secrets),
                                     np.sum(shares, axis=0))
        shares_str: List[List[str]] = np.vectorize(
            Share.__to_str)([s1, *shares]).tolist()
        return shares_str

    @sharize.register(Dim2)
    @staticmethod
    def __sharize_2dimension(secrets: List[List[Union[float, int]]],
                             party_size: int = 3) -> List[List[List[str]]]:
        """ 2次元リストのシェア化 """
        transposed: List[Union[List[int], List[float]]] \
            = np.array(secrets, dtype=object).transpose().tolist()
        dst: List[List[List[str]]] = [
            Share.sharize(col, party_size) for col in transposed
        ]
        dst = np.array(dst, dtype=object).transpose(1, 2, 0).tolist()

        return dst

    @sharize.register(dict)
    @staticmethod
    def __sharize_dict(secrets: dict, party_size: int = 3) -> List[dict]:
        """ 辞書型のシェア化 """
        shares_str: List[dict] = [dict() for _ in range(party_size)]
        for key, val in secrets.items():
            for i, share_val in enumerate(Share.sharize(val, party_size)):
                shares_str[i][key] = share_val
        return shares_str

    @sharize.register(DictList)
    @staticmethod
    def __sharize_dictlist(secrets: dict, party_size: int = 3) \
            -> List[List[dict]]:
        """ 辞書型配列のシェア化 """
        shares_str: List[List[dict]] = [[] for _ in range(party_size)]
        for secret_dict in secrets:
            share_dict: List[dict] = Share.sharize(secret_dict, party_size)
            for ss, sd in zip(shares_str, share_dict):
                ss.append(sd)
        return shares_str

    @recons.register(Dim1)
    @staticmethod
    def __recons_list1(shares: List[Union[int, Decimal]]):
        """ 1次元リストのシェアを復元 """
        return sum(shares)

    @recons.register(Dim2)
    @recons.register(Dim3)
    @staticmethod
    def __recons_list(shares: List[List[Union[int, Decimal]]]) -> List:
        """ リストのシェアを復元 """
        secrets: List = [
            Share.recons([shares_pi[i] for shares_pi in shares])
            for i in range(len(shares[0]))
        ]
        return secrets

    @recons.register(DictList)
    @staticmethod
    def __recons_dictlist(shares: List[dict]) -> dict:
        """ 辞書型を復元 """
        secrets: dict = dict()
        for key in shares[0].keys():
            val = []
            for s in shares:
                val.append(s[key])
            secrets[key] = Share.recons(val)
        return secrets

    @recons.register(DictList2)
    @staticmethod
    def __recons_dictlist2(shares: List[List[dict]]) -> list:
        """ 辞書型配列を復元 """
        secrets: list = list()
        for i in range(len(shares[0])):
            val = []
            for s in shares:
                val.append(s[i])
            secrets.append(Share.recons(val))
        return secrets

    @staticmethod
    def get_pre_convert_func(
            schema: Optional[Schema]) -> Callable[[str], Any]:
        """ スキーマに合った変換関数を返す  """
        if schema is None:
            return Decimal
        type = schema.type
        if type == ShareValueTypeEnum.Value('SHARE_VALUE_TYPE_UNSPECIFIED'):
            return Decimal
        if type == ShareValueTypeEnum.Value('SHARE_VALUE_TYPE_FIXED_POINT'):
            return Decimal
        if type == ShareValueTypeEnum.Value(
                'SHARE_VALUE_TYPE_UTF_8_INTEGER_REPRESENTATION'):
            # TODO: Decimal の経由を不要にする
            return lambda x: int(Decimal(x))
        return float

    @staticmethod
    def convert_int_to_str(x: int):
        bytes_repr: bytes = x.to_bytes((x.bit_length() + 7) //
                                       8, byteorder='big')
        str_repr: str = bytes_repr.decode('utf-8')
        return str_repr

    @staticmethod
    def get_convert_func(
            schema: Optional[Schema]) -> Callable[[Any], Any]:
        """ スキーマに合った変換関数を返す  """
        if schema is None:
            return float
        type = schema.type
        if type == ShareValueTypeEnum.Value('SHARE_VALUE_TYPE_UNSPECIFIED'):
            return float
        if type == ShareValueTypeEnum.Value('SHARE_VALUE_TYPE_FIXED_POINT'):
            return float
        if type == ShareValueTypeEnum.Value(
                'SHARE_VALUE_TYPE_UTF_8_INTEGER_REPRESENTATION'):
            return Share.convert_int_to_str
        return float

    @convert_type.register(str)
    @staticmethod
    def __pre_convert_type_str(
            value: str, schema: Optional[Schema] = None) -> list:
        func = Share.get_pre_convert_func(schema)
        return func(value)

    @convert_type.register(Dim1)
    @staticmethod
    def __convert_type_list(
            values: List[Any],
            schema: Optional[Sequence[Optional[Schema]]] = None) -> list:
        if schema is None:
            schema = [None] * len(values)
        return [Share.convert_type(x, sch)
                for x, sch in zip(values, schema)]

    @convert_type.register(Decimal)
    @convert_type.register(int)
    @staticmethod
    def __convert_type_elem(
            value: Union[Decimal, int],
            schema: Optional[Schema] = None) -> Union[float, str]:
        func = Share.get_convert_func(schema)
        return func(value)

    @convert_type.register(Dim2)
    @staticmethod
    def __convert_type_table(
            table: List[List],
            schema: Optional[List[Schema]] = None) -> list:
        return [Share.convert_type(row, schema) for row in table]
