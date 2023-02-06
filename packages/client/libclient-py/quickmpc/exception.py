from dataclasses import dataclass
from typing import Union
from google.protobuf.reflection import GeneratedProtocolMessageType

from .proto.common_types.common_types_pb2 import JobStatus, JobErrorInfo


@dataclass(frozen=True)
class QMPCJobError(Exception):
    err_info: JobErrorInfo


@dataclass(frozen=True)
class QMPCServerError(Exception):
    err_info: Union[str, JobErrorInfo]


class ArgmentError(Exception):
    pass
