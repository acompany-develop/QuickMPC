import logging
import time
from dataclasses import dataclass
from enum import Enum
from functools import update_wrapper
from typing import Callable, List

import pandas as pd

from quickmpc.request import QMPCRequestInterface

from ..exception import QMPCJobError
from ..proto.common_types.common_types_pb2 import JobErrorInfo, JobStatus
from ..utils.overload_tools import Dim1, methoddispatch
from .progress import Progress

logger = logging.getLogger(__name__)


class ShareDataFrameStatus(Enum):
    OK = 1
    EXECUTE = 2
    ERROR = 3


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

    def wrapper(self: "ShareDataFrame", *args,
                progress: bool = False, **kwargs):
        self._wait_execute(progress)
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
    __qmpc_request: quickmpc.request.QMPCClientInterface
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

    def _wait_execute(self, progress: bool) -> None:
        if self.__status == ShareDataFrameStatus.ERROR:
            raise QMPCJobError("ShareDataFrame's status is `ERROR`")
        if self.__status == ShareDataFrameStatus.EXECUTE:
            logger.info("wait execute...")
            # TODO: 待機設定を指定できるようにする
            p = Progress()
            while True:
                res = self.__qmpc_request.get_computation_status(self.__id)
                if progress:
                    p.update(res)
                # ERRORがあればraise
                if any([s == JobStatus.ERROR for s in res.job_statuses]):
                    self.__status = ShareDataFrameStatus.ERROR
                    res_err = self.__qmpc_request.get_job_error_info(self.__id)
                    for info in res_err.job_error_info:
                        if info:
                            raise QMPCJobError(info)
                    raise QMPCJobError("")
                # 全てCOMPLETEDならreturn
                if all([s == JobStatus.COMPLETED for s in res.job_statuses]):
                    self.__status = ShareDataFrameStatus.OK
                    return
                time.sleep(1)

    def __add__(self, other: "ShareDataFrame") -> "ShareDataFrame":
        """テーブルを加算する．

        qmpc.send_toで送ったデータでかつ，行数，列数が一致している場合のみ正常に動作する．

        Parameters
        ----------
        other: ShareDataFrame
            結合したいDataFrame

        Returns
        ----------
        Result
            加算して得られたDataFrameのResult
        """
        res = self.__qmpc_request.add_share_data_frame(self.__id, other.__id)
        return ShareDataFrame(res.data_id, self.__qmpc_request)

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
        progress: bool
            計算の進捗ログを出力するかどうか

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
        progress: bool
            計算の進捗ログを出力するかどうか

        Returns
        ----------
        pd.DataFrame
            計算結果
        """
        # 計算結果でないなら取得できないようにする
        if not self.__is_result:
            raise RuntimeError("Shareを取り出すことはできません．")
        res = self.__qmpc_request.get_computation_result(self.__id, None)
        return res.results

    @_wait_execute_decorator
    def get_error_info(self) -> List[JobErrorInfo]:
        """計算中のエラー情報を取得する

        Parameters
        ----------
        progress: bool
            計算の進捗ログを出力するかどうか

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
