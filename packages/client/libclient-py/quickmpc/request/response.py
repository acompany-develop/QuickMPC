from dataclasses import dataclass
from typing import List

import pandas as pd

from ..proto.common_types.common_types_pb2 import (JobErrorInfo,
                                                   JobProgress, JobStatus)


@dataclass(frozen=True)
class SendShareResponse():
    data_id: str


@dataclass(frozen=True)
class ExecuteResponse():
    job_uuid: str


@dataclass(frozen=True)
class GetResultResponse():
    results: pd.DataFrame


@dataclass(frozen=True)
class GetJobErrorInfoResponse():
    job_error_info: List[JobErrorInfo]


@dataclass(frozen=True)
class GetComputationStatusResponse():
    job_statuses: List[JobStatus.ValueType]
    progresses: List[JobProgress]


@dataclass(frozen=True)
class GetElapsedTimeResponse():
    elapsed_time: List[float]


@dataclass(frozen=True)
class AddShareDataFrameResponse():
    data_id: str
