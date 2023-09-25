import csv
from dataclasses import dataclass
from hashlib import sha512
from typing import Dict, Iterable, List, Optional, Sequence, Tuple, Union

import numpy as np

from quickmpc.exception import ArgumentError
from quickmpc.proto.common_types.common_types_pb2 import (Schema,
                                                          ShareValueTypeEnum)
from quickmpc.qmpc_logging import get_logger
from quickmpc.utils import Dim1, methoddispatch

logger = get_logger()


SUPPORT_TAGS: Dict[str, ShareValueTypeEnum.ValueType] = {
    'id': ShareValueTypeEnum.Value('SHARE_VALUE_TYPE_FIXED_POINT'),
}

ShareValueType = Union[float, int]


@dataclass(frozen=True)
class FormatChecker:
    """Formatが正しいかをチェックする
    """
    @methoddispatch(is_static_method=True)
    @staticmethod
    def check_duplicate(*args, **kw):
        raise ArgumentError("不正な引数が与えられています．")

    @staticmethod
    @check_duplicate.register(Dim1)
    def check_duplicate_dummy(*args, **kw):
        raise ArgumentError("不正な引数が与えられています．")

    @check_duplicate.register((Dim1, str))
    @staticmethod
    def check_duplicate_strs(lst: List[str]) -> bool:
        """文字列配列に重複要素があるかチェックする

        Parameters
        ----------
        lst: List[str]
            チェックする配列

        Returns
        -------
        bool
            違反してないかどうか
        """
        return len(lst) == len(set(lst))

    @check_duplicate.register((Dim1, Schema))
    @staticmethod
    def check_duplicate_typed(schema: List[Schema]) -> bool:
        """スキーマ配列に重複要素があるかチェックする

        スキーマ種別による区別はなく，スキーマの名前だけで重複チェックを行う

        Parameters
        ----------
        schema: List[str]
            チェックする配列

        Returns
        -------
        bool
            違反してないかどうか
        """
        return len(schema) == len(set([sch.name for sch in schema]))

    @staticmethod
    def check_size(secrets: Sequence[Sequence[Union[str, ShareValueType]]],
                   schema: Sequence[Union[str, Schema]]) -> np.bool_:
        """Schemaサイズとテーブルの列サイズが等しいかをチェックする

        Parameters
        ----------
        secrets: Sequence[Sequence[Union[str, ShareValueType]]]
            チェックするテーブルデータ
        schema: Sequence[Union[str, Schema]])
            チェックするスキーマ

        Returns
        -------
        np.bool_
            違反してないかどうか
        """
        return np.all([len(s) == len(schema) for s in secrets])


def format_check(secrets: List[List[ShareValueType]],
                 schema: Sequence[Union[str, Schema]]) -> bool:
    """Schemaとテーブルデータが要件を満たすかチェックする

    Parameters
    ----------
    secrets: List[List[ShareValueType]],
        チェックするテーブルデータ
    schema: Sequence[Union[str, Schema]])
        チェックするスキーマ

    Returns
    -------
    bool
        違反してないかどうか
    """
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
    """stringをfloatに変換する

    stringが数値に変換可能な場合は数値に変換する．
    変換できない場合は512bitのhashに変換した上で上位k bitを切り取り，
    整数部(k-m) bit，小数部m bitの値をfloatに変換して返す．
    ただし，k,mはMPCサーバ内部のLTZ(Less Than Zero)プロトコルで使用されるハイパーパラメータである．

    Parameters
    ----------
    val: str
        変換する文字列

    Returns
    -------
    float
        変換された実数

    Examples
    ---------
    >>> to_float("1")
    1.0
    >>> to_float("1.01")
    1.01
    >>> to_float("string")
    41254067.792910576
    """
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
    """stringをintに変換する

    文字列を `encoding` の形式で数値に変換する．
    元々数値に変換できる文字列だったとしても全て `encoding` の形式に変換する．

    Parameters
    ----------
    val: str
        変換する文字列
    encoding: str, default="utf-8"
        変換形式

    Returns
    -------
    int
        変換された実数

    Examples
    ---------
    >>> to_int("1")
    49
    >>> to_int("1.01")
    825110577
    >>> to_int("string")
    126943972912743
    """
    encoded = val.encode(encoding)
    return int.from_bytes(encoded, byteorder='big')


def check_float_data(val: str) -> bool:
    """文字列がfloatに変換可能であるかチェックする

    Parameters
    ----------
    val: str
        チェックする文字列

    Returns
    -------
    bool
        変換可能かどうか
    """
    try:
        _ = float(val)
        return True
    except ValueError:
        return False


def find_type(col_schema: str,
              col_data: List[str], is_matching_column: bool) \
        -> ShareValueTypeEnum.ValueType:
    """スキーマにタグを検索する

    スキーマ名に `:<tag>` という形式のタグがついている場合はこのタグを返す．
    ついていない場合はテーブルデータの文字列から推測する．

    Parameters
    ----------
    col_schema: str,
        スキーマの名前
    col_data: List[str],
        列の全てのデータ
    is_matching_column: bool)
        その列がID列かどうか

    Returns
    -------
    ShareValueTypeEnum.ValueType
        スキーマのタグ

    Examples
    --------
    >>> find_type("s:id", ["a"], True)
    ShareValueTypeEnum.SHARE_VALUE_TYPE_FIXED_POINT
    >>> find_type("s", ["a"], True)
    ShareValueTypeEnum.SHARE_VALUE_TYPE_FIXED_POINT
    >>> find_type("s", ["a"], False)
    ShareValueTypeEnum.SHARE_VALUE_TYPE_UTF_8_INTEGER_REPRESENTATION
    >>> find_type("s", ["100"], False)
    ShareValueTypeEnum.SHARE_VALUE_TYPE_FIXED_POINT
    """
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
    """各スキーマのタグを検索する

    Parameters
    ----------
    schema: List[str]
        テーブルデータのスキーマ
    data: List[List[str]]
        テーブルデータ
    matching_column: Optional[int], default=None
        ID列のindex(1-index)

    Returns
    -------
    List[ShareValueTypeEnum.ValueType]
        各スキーマのタグ
    """
    # transpose to get column oriented list
    transposed: Iterable[List[str]] = map(list, zip(*data))
    return [find_type(sch, col, idx == matching_column)
            for idx, (sch, col) in enumerate(zip(schema, transposed), start=1)]


def convert(element: str,
            type_info: ShareValueTypeEnum.ValueType) -> ShareValueType:
    """スキーマのタグに沿って文字列を数値に変換する

    Parameters
    ----------
    element: str
        変換する文字列
    type_info: ShareValueTypeEnum.ValueType
        スキーマタグ

    Returns
    -------
    ShareValueType
        変換後の値
    """
    if type_info == ShareValueTypeEnum.Value('SHARE_VALUE_TYPE_FIXED_POINT'):
        return to_float(element)
    if type_info == ShareValueTypeEnum.Value(
            'SHARE_VALUE_TYPE_UTF_8_INTEGER_REPRESENTATION'):
        return to_int(element)
    return to_float(element)


def parse(data: List[List[str]], matching_column: Optional[int] = None) \
        -> Tuple[List[List[ShareValueType]], List[Schema]]:
    """テーブルデータをMPCで使用できる形式にparseする

    Parameters
    ----------
    data: List[List[str]]
        テーブルデータ
    matching_column: Optional[int], default=None
        ID列のindex(1-index)

    Returns
    -------
    Tuple[List[List[ShareValueType]], List[Schema]]:
        parseしたスキーマとテーブルデータ
    """
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
    """csvのテーブルデータをMPCで使用できる形式にparseする

    Parameters
    ----------
    filename: str
        入力ファイルのpath
    matching_column: Optional[int], default=None
        ID列のindex(1-index)

    Returns
    -------
    Tuple[List[List[ShareValueType]], List[Schema]]:
        parseしたスキーマとテーブルデータ
    """
    with open(filename) as f:
        reader = csv.reader(f)
        text: List[List[str]] = [row for row in reader]
        return parse(text, matching_column)
