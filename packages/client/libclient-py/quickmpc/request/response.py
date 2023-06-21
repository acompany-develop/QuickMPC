from dataclasses import dataclass
from typing import Any, List, Optional

from ..proto.common_types.common_types_pb2 import (JobErrorInfo,
                                                   JobProgress, JobStatus)
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
    job_status: Optional[List[JobStatus]]
    progress: List[JobProgress]
    results: Any  # TODO: 適切な型を付ける


@dataclass(frozen=True)
class GetJobErrorInfoResponse():
    status: Status
    job_error_info: List[JobErrorInfo]
