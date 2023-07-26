from dataclasses import dataclass
from decimal import Decimal
from typing import (Any, Callable, ClassVar, List,
                    Optional, Sequence, Tuple, Union)

import numpy as np

from quickmpc.exception import ArgumentError
from quickmpc.proto.common_types.common_types_pb2 import (Schema,
                                                          ShareValueTypeEnum)
from quickmpc.qmpc_logging import get_logger
from quickmpc.share.random import ChaCha20, RandomInterface
from quickmpc.utils import (DictList, DictList2, Dim1,
                            Dim2, Dim3, methoddispatch)

logger = get_logger()


@dataclass(frozen=True)
class Share:
    __SHIFT_VAL = 10**8
    __SECRET_RANGE = 10**19 * __SHIFT_VAL

    Scalar = Union[int, float, Decimal]

    @methoddispatch(is_static_method=True)
    @staticmethod
    def sharize(_, __):
        logger.error("Invalid argument on sharize.")
        raise ArgumentError("不正な引数が与えられています．")

    @sharize.register(Scalar)
    @staticmethod
    def __sharize_scalar(secret: Scalar, party_size: int = 3) -> List[str]:
        """ スカラ値のシェア化 """
        secret *= Share.__SHIFT_VAL
        if abs(secret) > Share.__SECRET_RANGE:
            logger.error("Out of range")
            raise ArgumentError("Out of range")
        rnd: RandomInterface = ChaCha20()
        shares: List[Decimal] = rnd.get_list(-Share.__SECRET_RANGE, Share.__SECRET_RANGE, party_size)
        shares[0] += Decimal(secret) - np.sum(shares)
        return [str(n / Share.__SHIFT_VAL) for n in shares]

    @sharize.register(List)
    @staticmethod
    def __sharize_multidim(secrets: List[Scalar], party_size: int = 3):
        return [Share.__sharize(secret, party_size) for secret in secrets]

    @methoddispatch(is_static_method=True)
    @staticmethod
    def recons(_):
        logger.error("Invalid argument on sharize.")
        raise ArgumentError("不正な引数が与えられています．")

    @recons.register(List[Scalar])
    @staticmethod
    def __recons_scalar(shares: List[Scalar]):
        return sum(shares)

    @recons.register(List[List])
    @staticmethod
    def __recons_multidim(multidim_shares: List[List]) -> List:
        return [Share.recons(shares) for shares in multidim_shares]

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

    @methoddispatch(is_static_method=True)
    @staticmethod
    def convert_type(_, __):
        logger.error("Invalid argument on convert_type.")
        raise ArgumentError("不正な引数が与えられています．")

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
