from dataclasses import dataclass
from typing import Union

from quickmpc.proto.common_types.common_types_pb2 import JobErrorInfo


@dataclass(frozen=True)
class QMPCJobError(Exception):
    """MPCの計算中にエラーが発生した場合のエラー

    Attributes
    ----------
    err_info: Union[str, JobErrorInfo]
        エラーの詳細
    """
    err_info: Union[str, JobErrorInfo]


@dataclass(frozen=True)
class QMPCServerError(Exception):
    """QMPCサーバで何らかの異常が発生した場合エラー

    Attributes
    ----------
    err_info: Union[str, JobErrorInfo]
        エラーの詳細
    """
    err_info: Union[str, JobErrorInfo]


class ArgumentError(Exception):
    """想定していない引数が与えられた時のエラー"""
    # TODO: NotImplementedErrorに置き換えられるので削除する
    pass
