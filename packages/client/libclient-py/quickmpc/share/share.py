import inspect
import threading
from dataclasses import dataclass
from decimal import Decimal, getcontext
from functools import wraps
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


# getcontext().precがthreadごとに初期化されてしまうため，
# theread毎に初期化し返せるようできるようにフラグを管理する
_local_dict = threading.local()
# Triple cのshareが23byteで約10^{54}なので, 余裕を持って収まるように100にしておく
_decimal_prec = 100


def set_decimal_prec():
    try:
        _local_dict.is_initialized
    except Exception:
        getcontext().prec = _decimal_prec
        _local_dict.is_initialized = True


set_decimal_prec()


def _verify_decimal_prec(f):
    """メソッドにprecのverifyを適用させるデコレータ"""
    @wraps(f)
    def _wrapper(*args, **kw):
        set_decimal_prec()
        if getcontext().prec != _decimal_prec:
            raise RuntimeError(
                f"Decimal context prec is must be {_decimal_prec}.")
        return f(*args, **kw)
    return _wrapper


def _verify_decimal_prec_all(cls):
    """クラスの全てのメソッドにprecのverifyを適用させるデコレータ"""
    for name, fn in inspect.getmembers(cls):
        if name.startswith('__'):
            continue
        if callable(getattr(cls, name)):
            setattr(cls, name, _verify_decimal_prec(fn))
    return cls


@_verify_decimal_prec_all
@dataclass(frozen=True)
class Share:
    """Share関連の処理をまとめているクラス

    Attributes
    ----------
    _Share__share_random_range: Tuple[Decimal, Decimal]
        share乱数の範囲
    """
    __share_random_range: ClassVar[Tuple[Decimal, Decimal]] =\
        (Decimal(-(1 << 64)), Decimal(1 << 64))

    @methoddispatch(is_static_method=True)
    @staticmethod
    def __to_str(*args, **kw):
        """文字列に変換する

        overloadして使用される．
        """
        raise ArgumentError("不正な引数が与えられています．")

    @__to_str.register(Decimal)
    @staticmethod
    def __decimal_to_str(val: Decimal) -> str:
        """decimalを文字列に変換する

        Parameters
        ----------
        val: Decimal
            変換する値

        Returns
        -------
        str
            変換された文字列
        """
        # InfinityをCCで読み込めるinfに変換
        return 'inf' if Decimal.is_infinite(val) else str(val)

    @__to_str.register(int)
    @staticmethod
    def __int_to_str(val: int) -> str:
        """intを文字列に変換する

        Parameters
        ----------
        val: int
            変換する値

        Returns
        -------
        str
            変換された文字列
        """
        return str(val)

    @methoddispatch(is_static_method=True)
    @staticmethod
    def sharize(*args, **kw):
        """sharizeする

        overloadして使用される．
        """
        raise ArgumentError("不正な引数が与えられています．")

    @methoddispatch(is_static_method=True)
    @staticmethod
    def recons(*args, **kw):
        """Shareを復元する

        overloadして使用される．
        """
        raise ArgumentError("不正な引数が与えられています．")

    @methoddispatch(is_static_method=True)
    @staticmethod
    def convert_type(*args, **kw):
        """値を変換する

        overloadして使用される．
        """
        raise ArgumentError("不正な引数が与えられています．")

    @sharize.register(int)
    @sharize.register(float)
    @staticmethod
    def __sharize_scalar(secrets: float, party_size: int = 3) -> List[str]:
        """スカラ値をシェア化する

        Parameters
        ----------
        secrets: float
            スカラ値
        party_size: int, defulat=3
            MPCのパーティ数

        Returns
        -------
        List[str]
            シェア
        """
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
        """実数の1次元リストをシェア化する

        Parameters
        ----------
        secrets: List[Union[float, Decimal]]
            1次元リスト
        party_size: int, defulat=3
            MPCのパーティ数

        Returns
        -------
        List[List[str]]
            シェアのリスト
        """
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
        """1次元リストをシェア化する

        Parameters
        ----------
        secrets: List[Decimal]
            1次元リスト
        party_size: int, defulat=3
            MPCのパーティ数

        Returns
        -------
        List[List[str]]
            シェアのリスト
        """
        return Share.__sharize_1dimension_float(secrets, party_size)

    @sharize.register((Dim1, int))
    @staticmethod
    def __sharize_1dimension_int(secrets: List[int], party_size: int = 3) \
            -> List[List[str]]:
        """整数の1次元リストをシェア化する

        Parameters
        ----------
        secrets: List[int]
            1次元リスト
        party_size: int, defulat=3
            MPCのパーティ数

        Returns
        -------
        List[List[str]]
            シェアのリスト
        """
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
        """2次元リストをシェア化する

        Parameters
        ----------
        secrets: List[int]
            1次元リスト
        party_size: int, defulat=3
            MPCのパーティ数

        Returns
        -------
        List[List[List[str]]]
            シェアのリスト
        """
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
        """辞書型をシェア化する

        Parameters
        ----------
        secrets: List[int]
            1次元リスト
        party_size: int, defulat=3
            MPCのパーティ数

        Returns
        -------
        List[dict]
            シェアのリスト
        """
        shares_str: List[dict] = [dict() for _ in range(party_size)]
        for key, val in secrets.items():
            for i, share_val in enumerate(Share.sharize(val, party_size)):
                shares_str[i][key] = share_val
        return shares_str

    @sharize.register(DictList)
    @staticmethod
    def __sharize_dictlist(secrets: dict, party_size: int = 3) \
            -> List[List[dict]]:
        """辞書型配列をシェア化する

        Parameters
        ----------
        secrets: List[int]
            1次元リスト
        party_size: int, defulat=3
            MPCのパーティ数

        Returns
        -------
        List[List[dict]]
            シェアのリスト
        """
        shares_str: List[List[dict]] = [[] for _ in range(party_size)]
        for secret_dict in secrets:
            share_dict: List[dict] = Share.sharize(secret_dict, party_size)
            for ss, sd in zip(shares_str, share_dict):
                ss.append(sd)
        return shares_str

    @recons.register(Dim1)
    @staticmethod
    def __recons_list1(shares: List[Union[int, Decimal]]):
        """1次元リストのシェアを復元

        Parameters
        ----------
        shares: List[Union[int, Decimal]]
            シェア

        Returns
        -------
        Union[int, Decimal]
            復元した値
        """
        return sum(shares)

    @recons.register(Dim2)
    @recons.register(Dim3)
    @staticmethod
    def __recons_list(shares: List[List[Union[int, Decimal]]]) -> List:
        """リストのシェアを復元

        Parameters
        ----------
        shares: List[List[Union[int, Decimal]]]
            シェア

        Returns
        -------
        List
            復元した値
        """
        secrets: List = [
            Share.recons([shares_pi[i] for shares_pi in shares])
            for i in range(len(shares[0]))
        ]
        return secrets

    @recons.register(DictList)
    @staticmethod
    def __recons_dictlist(shares: List[dict]) -> dict:
        """辞書型のシェアを復元

        Parameters
        ----------
        shares: List[dict]
            シェア

        Returns
        -------
        dict
            復元した値
        """
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
        """辞書型配列のシェアを復元

        Parameters
        ----------
        shares: List[List[dict]]
            シェア

        Returns
        -------
        List[dict]
            復元した値
        """
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
        """スキーマに合った変換関数を返す

        Parameters
        ----------
        schema: Optional[Schema])
            スキーマ

        Returns
        -------
        Callable[[str], Any]:
            変換関数
        """
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
        """intをstrに変換する

        Parameters
        ----------
        x: int
            整数

        Returns
        -------
        str
            変換した文字列
        """
        bytes_repr: bytes = x.to_bytes((x.bit_length() + 7) //
                                       8, byteorder='big')
        str_repr: str = bytes_repr.decode('utf-8')
        return str_repr

    @staticmethod
    def get_convert_func(
            schema: Optional[Schema]) -> Callable[[Any], Any]:
        """スキーマに合った変換関数を返す

        Parameters
        ----------
        schema: Optional[Schema])
            スキーマ

        Returns
        -------
        Callable[[str], Any]:
            変換関数
        """
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
    def __pre_convert_type_str(value: str, schema: Optional[Schema] = None) \
            -> Union[int, float, Decimal]:
        """文字列をスキーマタグに沿って数値に変換する

        Parameters
        ----------
        value: str
            変換する文字列
        schema: Optional[Schema], default=None
            スキーマ

        Returns
        -------
        Union[int, float, Decimal]:
            変換した数値
        """
        func = Share.get_pre_convert_func(schema)
        return func(value)

    @convert_type.register(Dim1)
    @staticmethod
    def __convert_type_list(
            values: List[Any],
            schema: Optional[Sequence[Optional[Schema]]] = None) -> list:
        """リストをスキーマタグに沿って数値に変換する

        Parameters
        ----------
        value: List[Any]
            変換する文字列
        schema: Optional[Sequence[Optional[Schema]]], default=None
            スキーマ

        Returns
        -------
        list
            変換した数値
        """
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
        """数値をスキーマタグに沿って数値に変換する

        Parameters
        ----------
        value: Union[Decimal, int]
            変換する文字列
        schema: Optional[Schema], default=None
            スキーマ

        Returns
        -------
        Union[float, int]
            変換した数値
        """
        func = Share.get_convert_func(schema)
        return func(value)

    @convert_type.register(Dim2)
    @staticmethod
    def __convert_type_table(
            table: List[List],
            schema: Optional[List[Schema]] = None) -> list:
        """テーブルデータをスキーマタグに沿って数値に変換する

        Parameters
        ----------
        value: Union[Decimal, int]
            変換する文字列
        schema: Optional[List[Schema]], default=None
            スキーマ

        Returns
        -------
        List
            変換した数値
        """
        return [Share.convert_type(row, schema) for row in table]
