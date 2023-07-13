from abc import ABC, abstractmethod
from typing import List, Optional

import pandas as pd

from quickmpc.request.response import (AddShareDataFrameResponse,
                                       ExecuteResponse,
                                       GetComputationStatusResponse,
                                       GetElapsedTimeResponse,
                                       GetJobErrorInfoResponse,
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
    def join(self, data_ids: List[str], *, debug_mode: bool) \
        -> ExecuteResponse: ...

    @abstractmethod
    def get_computation_result(self, job_uuid: str,
                               output_path: Optional[str]) \
        -> GetResultResponse: ...

    @abstractmethod
    def get_job_error_info(self, job_uuid: str) -> GetJobErrorInfoResponse: ...

    @abstractmethod
    def get_computation_status(self, job_uuid: str) \
        -> GetComputationStatusResponse: ...

    @abstractmethod
    def get_elapsed_time(self, job_uuid: str) -> GetElapsedTimeResponse: ...

    @abstractmethod
    def delete_share(self, data_ids: List[str]) -> None: ...

    @abstractmethod
    def add_share_data_frame(self, base_data_id: str, add_data_id: str) \
        -> AddShareDataFrameResponse: ...
