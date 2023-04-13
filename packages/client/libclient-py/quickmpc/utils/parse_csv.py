import csv
import logging
from dataclasses import dataclass
from hashlib import sha512
from typing import Dict, List, Optional, Sequence, Tuple, Union

import numpy as np

from ..exception import ArgumentError
from ..proto.common_types.common_types_pb2 import Schema, ShareValueTypeEnum
from .overload_tools import Dim1, methoddispatch

logger = logging.getLogger(__name__)

SUPPORT_TAGS: Dict[str, ShareValueTypeEnum.ValueType] = {
    'id': ShareValueTypeEnum.Value('SHARE_VALUE_TYPE_FIXED_POINT'),
}

ShareValueType = Union[float, int]


@dataclass(frozen=True)
class FormatChecker:
    @methoddispatch(is_static_method=True)
    @staticmethod
    def check_duplicate(_):
        raise ArgumentError("不正な引数が与えられています．")

    @check_duplicate.register(Dim1)
    @staticmethod
    def check_duplicate_dummy(schema: List[str]):
        raise ArgumentError("不正な引数が与えられています．")

    @check_duplicate.register((Dim1, str))
    @staticmethod
    def check_duplicate_strs(schema: List[str]) -> bool:
        return len(schema) == len(set(schema))

    @check_duplicate.register((Dim1, Schema))
    @staticmethod
    def check_duplicate_typed(schema: List[Schema]) -> bool:
        return len(schema) == len(set([sch.name for sch in schema]))

    @staticmethod
    def check_size(secrets: Sequence[Sequence[Union[str, ShareValueType]]],
                   schema: Sequence[Union[str, Schema]]) -> np.bool_:
        return np.all([len(s) == len(schema) for s in secrets])


def format_check(secrets: List[List[ShareValueType]],
                 schema: Sequence[Union[str, Schema]]) -> bool:
    # 存在チェック
    if not (schema and secrets):
        logger.error("Schema or secrets table are not exists.")
        return False
    # 重複チェック
    if not FormatChecker.check_duplicate(schema):
        logger.error("Duplicate schema name.")
        return False
    # サイズチェック
    if not FormatChecker.check_size(secrets, schema):
        logger.error("schema size and table colummn size are different.")
        return False
    return True


def to_float(val: str) -> float:
    """ If val is a float, convert as is; if it is a string, hash it. """
    try:
        return float(val)
    except ValueError:
        # k,m are constants used in the comparison operation
        # Due to the limitation of comparison operation,
        # k bits are taken out and divided by 2^m.
        k: int = 48
        m: int = 20
        hs: str = sha512(val.encode()).hexdigest()
        val_int: int = int(hs[:(k >> 2)], 16)
        val_float: float = val_int / pow(2, m)
        return val_float


def to_int(val: str, encoding='utf-8') -> int:
    encoded = val.encode(encoding)
    return int.from_bytes(encoded, byteorder='big')


def check_float_data(val: str) -> bool:
    try:
        _ = float(val)
        return True
    except ValueError:
        return False


def find_type(col_schema: str,
              col_data: List[str], is_matching_column: bool) \
        -> ShareValueTypeEnum.ValueType:
    # check tag
    type_str, *remains = col_schema.split(':')

    if len(remains) > 0:
        if type_str in SUPPORT_TAGS:
            return SUPPORT_TAGS[type_str]

    # check if the column of interest is the one used for matching
    if is_matching_column:
        return ShareValueTypeEnum.SHARE_VALUE_TYPE_FIXED_POINT

    # if even one string is not considered a floating point value,
    # treat the column as string data
    res_type = ShareValueTypeEnum.SHARE_VALUE_TYPE_FIXED_POINT

    if not all([check_float_data(v) for v in col_data]):
        res_type = \
            ShareValueTypeEnum.SHARE_VALUE_TYPE_UTF_8_INTEGER_REPRESENTATION

    return res_type


def find_types(schema: List[str],
               data: List[List[str]],
               matching_column: Optional[int] = None
               ) -> List[ShareValueTypeEnum.ValueType]:
    # transpose to get column oriented list
    transposed: List[List[str]] = np.array(
        data, dtype=str).transpose().tolist()
    return [find_type(sch, col, idx == matching_column)
            for idx, (sch, col) in enumerate(zip(schema, transposed), start=1)]


def convert(element: str,
            type_info: ShareValueTypeEnum.ValueType) -> ShareValueType:
    if type_info == ShareValueTypeEnum.Value('SHARE_VALUE_TYPE_FIXED_POINT'):
        return to_float(element)
    if type_info == ShareValueTypeEnum.Value(
            'SHARE_VALUE_TYPE_UTF_8_INTEGER_REPRESENTATION'):
        return to_int(element)
    return to_float(element)


def parse(data: List[List[str]], matching_column: Optional[int] = None) \
        -> Tuple[List[List[ShareValueType]], List[Schema]]:
    schema_name: List[str] = data[0]
    types = find_types(schema_name, data[1:], matching_column)
    schema = [Schema(name=name, type=type)
              for name, type in zip(schema_name, types)]

    # check size first because an iterator which `zip` bultin function returns
    # stops when the shortest iterable is exhausted
    if not FormatChecker.check_size(data[1:], schema):
        logger.error("schema size and table colummn size are different.")
        raise RuntimeError("規定されたフォーマットでないデータです．")

    secrets: List[List[ShareValueType]] = [
        [convert(x, t) for x, t in zip(row, types)] for row in data[1:]]

    if not format_check(secrets, schema):
        raise RuntimeError("規定されたフォーマットでないデータです．")

    return secrets, schema


def parse_csv(
    filename: str, matching_column: Optional[int] = None) \
        -> Tuple[List[List[ShareValueType]], List[Schema]]:
    with open(filename) as f:
        reader = csv.reader(f)
        text: List[List[str]] = [row for row in reader]
        return parse(text, matching_column)
