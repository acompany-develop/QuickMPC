from abc import ABC, abstractmethod
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
    def send_share(self, df: pd.DataFrame,
                   piece_size: int) -> SendShareResponse: ...

    @abstractmethod
    def sum(self, data_ids: List[str], columns: List[int]) \
        -> ExecuteResponse: ...

    @abstractmethod
    def mean(self, data_ids: List[str], columns: List[int]) \
        -> ExecuteResponse: ...

    @abstractmethod
    def variance(self, data_ids: List[str], columns: List[int]) \
        -> ExecuteResponse: ...

    @abstractmethod
    def correl(self, data_ids: List[str], columns1: List[int],
               columns2: List[int]) -> ExecuteResponse: ...

    @abstractmethod
    def meshcode(self, data_ids: List[str], columns: List[int]) \
        -> ExecuteResponse: ...

    @abstractmethod
    def join(self, data_ids: List[str]) \
        -> ExecuteResponse: ...

    @abstractmethod
    def get_computation_result(self, job_uuid: str, output_path: str) \
        -> GetResultResponse: ...

    @abstractmethod
    def get_job_error_info(self, job_uuid: str) -> GetJobErrorInfoResponse: ...
