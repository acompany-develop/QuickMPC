import logging
import time
from dataclasses import dataclass
from enum import Enum
from functools import update_wrapper
from typing import Callable, List

import pandas as pd

from .proto.common_types.common_types_pb2 import JobErrorInfo, JobStatus
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
            while True:
                time.sleep(1)
                res = self.__qmpc_request.get_computation_result(self.__id)
                all_completed = all([
                    s == JobStatus.COMPLETED for s in res.job_statuses
                ])
                if all_completed:
                    break
            self.__status = ShareDataFrameStatus.OK

    @methoddispatch()
    def join(self, other: "ShareDataFrame", *, debug_mode=False) \
            -> "ShareDataFrame":
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
        return self.join([other], debug_mode=debug_mode)

    @join.register(Dim1)
    def join_list(self, others: List["ShareDataFrame"], *, debug_mode=False)\
            -> "ShareDataFrame":
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
        res = self.__qmpc_request.join([self.__id] + [o.__id for o in others],
                                       debug_mode=debug_mode)
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
        if res.results is None:
            return pd.DataFrame()
        # TODO: 型をつけて分岐できるようにする or そもそもdfで返す
        if type(res.results) == dict:
            if res.results["schema"] is None or res.results["table"] is None:
                return pd.DataFrame()
            schema = [s.name for s in res.results["schema"]]
            df = pd.DataFrame(res.results["table"],
                              columns=schema)
            return df
        return pd.DataFrame(res.results)

    @_wait_execute_decorator
    def get_error_info(self) -> List[JobErrorInfo]:
        """計算中のエラー情報を取得する

        Parameters
        ----------

        Returns
        ----------
        str
            エラー情報の文字列
        """
        # 計算していない場合はそもそもエラー情報は存在しない
        if not self.__is_result:
            return []
        res = self.__qmpc_request.get_job_error_info(self.__id)
        return res.job_error_info

    @_wait_execute_decorator
    def get_elapsed_time(self) -> List[float]:
        """計算時間を取得する

        Parameters
        ----------

        Returns
        ----------
        float
            計算時間
        """
        # 計算していない場合は計算時間が存在しない
        if not self.__is_result:
            raise RuntimeError("計算していないため計算時間を取得できません")
        res = self.__qmpc_request.get_elapsed_time(self.__id)
        return res.elapsed_time

    def get_id(self) -> str:
        """IDを取得する

        Parameters
        ----------

        Returns
        ----------
        str
            ID
        """
        return self.__id