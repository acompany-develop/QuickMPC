from dataclasses import dataclass
from typing import Union

from quickmpc.proto.common_types.common_types_pb2 import JobErrorInfo


@dataclass(frozen=True)
class QMPCJobError(Exception):
    err_info: Union[str, JobErrorInfo]


@dataclass(frozen=True)
class QMPCServerError(Exception):
    err_info: Union[str, JobErrorInfo]


class ArgumentError(Exception):
    pass
