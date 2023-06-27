from dataclasses import dataclass

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


@dataclass(frozen=True)
class GetJobErrorInfoResponse():
    status: Status
