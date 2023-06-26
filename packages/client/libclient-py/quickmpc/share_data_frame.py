import logging
import time
from dataclasses import dataclass
from enum import Enum
from functools import update_wrapper
from typing import Callable, List

import pandas as pd

from .proto.common_types.common_types_pb2 import JobStatus
from .request.qmpc_request_interface import QMPCRequestInterface
from .utils.overload_tools import Dim1, methoddispatch

logger = logging.getLogger(__name__)


class ShareDataFrameStatus(Enum):
    OK = 1
    EXECUTE = 2


def _wait_execute_decorator(func: Callable) -> Callable:
    """計算が終了するまで待機するデコレータ

    Parameters
    ----------
    func: Callable
        実行前に計算を待機させたいmethod

    Returns
    ----------
    Callable
        デコレータ関数
    """

    def wrapper(self: "ShareDataFrame", *args, **kwargs):
        self._wait_execute()
        return func(self, *args, **kwargs)
    update_wrapper(wrapper, func)
    return wrapper


@dataclass
class ShareDataFrame:
    """テーブルデータを管理するクラス

    Attributes
    ----------
    __id: str
        データのID
    __qmpc_request: QMPCClientInterface
        QuickMPCとの通信を担うClient
    __is_result: bool
        send由来のDataFrame(False)なのかexecute由来のDataFrameなのか(True)
    __status: ShareDataFrameStatus
        現在の状態
    """

    __id: str
    __qmpc_request: QMPCRequestInterface
    __is_result: bool = False
    __status: ShareDataFrameStatus = ShareDataFrameStatus.OK

    def _wait_execute(self):
        if self.__status == ShareDataFrameStatus.EXECUTE:
            logger.info("wait execute...")
            # TODO: 待機設定を指定できるようにする
            for _ in range(10):
                time.sleep(1)
                res = self.__qmpc_request.get_computation_result(self.__id)
                all_completed = all([
                    s == JobStatus.COMPLETED for s in res.job_statuses
                ])
                if all_completed:
                    break
            self.__status = ShareDataFrameStatus.OK

    @methoddispatch()
    def join(self, other: "ShareDataFrame") -> "ShareDataFrame":
        """テーブルデータを結合する．

        inner_joinのみ．

        Parameters
        ----------
        other: ShareDataFrame
            結合したいDataFrame

        Returns
        ----------
        Result
            結合したDataFrameのResult
        """
        return self.join([other])

    @join.register(Dim1)
    def join_list(self, others: List["ShareDataFrame"]) -> "ShareDataFrame":
        """テーブルデータを結合する．

        inner_joinのみ．

        Parameters
        ----------
        others: List[ShareDataFrame]
            結合したいDataFrameのリスト

        Returns
        ----------
        Result
            結合したDataFrameのResult
        """
        res = self.__qmpc_request.join([self.__id] + [o.__id for o in others])
        return ShareDataFrame(res.job_uuid, self.__qmpc_request,
                              True, ShareDataFrameStatus.EXECUTE)

    def sum(self, columns: list) -> "ShareDataFrame":
        res = self.__qmpc_request.sum([self.__id], columns)
        return ShareDataFrame(res.job_uuid, self.__qmpc_request,
                              True, ShareDataFrameStatus.EXECUTE)

    def mean(self, columns: list) -> "ShareDataFrame":
        res = self.__qmpc_request.mean([self.__id], columns)
        return ShareDataFrame(res.job_uuid, self.__qmpc_request,
                              True, ShareDataFrameStatus.EXECUTE)

    def variance(self, columns: list) -> "ShareDataFrame":
        res = self.__qmpc_request.variance([self.__id], columns)
        return ShareDataFrame(res.job_uuid, self.__qmpc_request,
                              True, ShareDataFrameStatus.EXECUTE)

    def correl(self, columns1: list, columns2: list) -> "ShareDataFrame":
        res = self.__qmpc_request.correl([self.__id], columns1, columns2)
        return ShareDataFrame(res.job_uuid, self.__qmpc_request,
                              True, ShareDataFrameStatus.EXECUTE)

    def meshcode(self, columns: list) \
            -> "ShareDataFrame":
        res = self.__qmpc_request.meshcode([self.__id], columns)
        return ShareDataFrame(res.job_uuid, self.__qmpc_request,
                              True, ShareDataFrameStatus.EXECUTE)

    @_wait_execute_decorator
    def to_csv(self, output_path: str) -> None:
        """計算結果をCSVに保存する．

        Parameters
        ----------
        output_path: str
            保存するファイルの絶対path

        Returns
        ----------
        """
        # 計算結果でないなら保存されないようにする
        if not self.__is_result:
            raise RuntimeError("Shareを保存することはできません．")
        self.__qmpc_request.get_computation_result(self.__id, output_path)

    @_wait_execute_decorator
    def to_data_frame(self) -> pd.DataFrame:
        """計算結果をDataFrameで返す

        Parameters
        ----------

        Returns
        ----------
        pd.DataFrame
            計算結果
        """
        # 計算結果でないなら取得できないようにする
        if not self.__is_result:
            raise RuntimeError("Shareを取り出すことはできません．")
        res = self.__qmpc_request.get_computation_result(self.__id, None)
        # TODO: 型をつけて分岐できるようにする or そもそもdfで返す
        if type(res.results) == dict:
            schema = [s.name for s in res.results["schema"]]
            df = pd.DataFrame(res.results["table"],
                              columns=schema)
            return df
        return pd.DataFrame(res.results)

    @_wait_execute_decorator
    def get_error_info(self) -> dict:
        """計算中のエラー情報を取得する

        Parameters
        ----------

        Returns
        ----------
        dict
            なんやかんや
        """
        ...
