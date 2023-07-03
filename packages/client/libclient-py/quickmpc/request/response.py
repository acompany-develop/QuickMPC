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
    results: Any  # TODO: 適切な型を付ける


@dataclass(frozen=True)
class GetJobErrorInfoResponse():
    status: Status
    job_error_info: List[JobErrorInfo]


@dataclass(frozen=True)
class GetComputationStatusResponse():
    status: Status
    job_statuses: List["JobStatus"]
    progresses: List[JobProgress]


@dataclass(frozen=True)
class GetElapsedTimeResponse():
    status: Status
    elapsed_time: List[float]


@dataclass(frozen=True)
class GetDataListResponse():
    status: Status
    data_ids: Optional[List[str]]


@dataclass(frozen=True)
class DeleteShareResponse():
    status: Status


@dataclass(frozen=True)
class AddShareDataFrameResponse():
    status: Status
    data_id: str
