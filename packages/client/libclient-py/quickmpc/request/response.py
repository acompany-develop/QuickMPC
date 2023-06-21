from dataclasses import dataclass
from typing import List

from ..proto.common_types.common_types_pb2 import JobStatus
from .status import Status


@dataclass(frozen=True)
class SendShareResponse():
    status: Status
    data_id: str


@dataclass(frozen=True)
class ExecuteResponse():
    status: Status
    job_uuid: str


@dataclass(frozen=True)
class GetResultResponse():
    status: Status
    job_status: List[JobStatus.Value]
    progress: List[int]
    results: dict  # TODO: 適切な型を付ける


@dataclass(frozen=True)
class GetJobErrorInfoResponse():
    status: Status
