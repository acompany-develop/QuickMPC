from abc import ABC, abstractmethod
from dataclasses import dataclass, field
from typing import List

import pandas as pd

from .response import (ExecuteResponse, GetJobErrorInfoResponse,
                       GetResultResponse, SendShareResponse)


class QMPCRequestInterface(ABC):
    """QuickMPCサーバと通信を行うインタフェース

    Attributes
    ----------
    """

    @abstractmethod
    def send_share(self, df: pd.DataFrame) -> SendShareResponse: ...

    @abstractmethod
    def sum(self, data_ids: List[str], inp: List[int]) -> ExecuteResponse: ...

    @abstractmethod
    def mean(self, data_ids: List[str], inp: List[int]) -> ExecuteResponse: ...

    @abstractmethod
    def variance(self, data_ids: List[str], inp: List[int]) \
        -> ExecuteResponse: ...

    @abstractmethod
    def correl(self, data_ids: List[str], inp1: List[int], inp2: List[int]) \
        -> ExecuteResponse: ...

    @abstractmethod
    def meshcode(self, data_ids: List[str], inp1: List[int], inp2: List[int]) \
        -> ExecuteResponse: ...

    @abstractmethod
    def join(self, data_ids: List[str]) \
        -> ExecuteResponse: ...

    @abstractmethod
    def get_computation_result(self, job_uuid: str, filepath: str) \
        -> GetResultResponse: ...

    @abstractmethod
    def get_job_error_info(self, job_uuid: str) -> GetJobErrorInfoResponse: ...


@dataclass(frozen=True)
class QMPCRequest(QMPCRequestInterface):
    """QuickMPCサーバと通信を行う

    Attributes
    ----------
    __endpoints: List[url]
        QuickMPCサーバのURL
    __token: str
        QuickMPCサーバへの通信を担う
    """

    __endpoints: List[str]
    __token: str = field(init=False)

    def send_share(self, df: pd.DataFrame) -> SendShareResponse: ...

    def sum(self, data_ids: List[str], inp: List[int]) -> ExecuteResponse: ...

    def mean(self, data_ids: List[str], inp: List[int]) -> ExecuteResponse: ...

    def variance(self, data_ids: List[str], inp: List[int]) \
        -> ExecuteResponse: ...

    def correl(self, data_ids: List[str], inp1: List[int], inp2: List[int]) \
        -> ExecuteResponse: ...

    def meshcode(self, data_ids: List[str], inp1: List[int], inp2: List[int]) \
        -> ExecuteResponse: ...

    def join(self, data_ids: List[str]) -> ExecuteResponse: ...

    def get_computation_result(self, job_uuid: str, filepath: str) \
        -> GetResultResponse: ...

    def get_job_error_info(self, job_uuid: str) -> GetJobErrorInfoResponse: ...
