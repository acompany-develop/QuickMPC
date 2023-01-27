from dataclasses import dataclass
from typing import Union
from google.protobuf.reflection import GeneratedProtocolMessageType


@dataclass(frozen=True)
class QMPCJobError(Exception):
    err_info: GeneratedProtocolMessageType


@dataclass(frozen=True)
class QMPCServerError(Exception):
    err_info: Union[str, GeneratedProtocolMessageType]


class ArgmentError(Exception):
    pass
