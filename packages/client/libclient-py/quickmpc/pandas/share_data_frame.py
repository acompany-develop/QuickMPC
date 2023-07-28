import logging
import time
from dataclasses import dataclass
from enum import Enum
from functools import update_wrapper
from typing import Callable, List

import pandas as pd

from quickmpc.exception import QMPCJobError
from quickmpc.pandas.progress import Progress
from quickmpc.proto.common_types.common_types_pb2 import (JobErrorInfo,
                                                          JobStatus)
from quickmpc.request import QMPCRequestInterface
from quickmpc.utils import Dim1, methoddispatch

logger = logging.getLogger(__name__)


class ShareDataFrameStatus(Enum):
    """Jobの計算Status
    """
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

    Args
    ----
    _ShareDataFrame__id: str
        データのID
    _ShareDataFrame__qmpc_request: quickmpc.request.QMPCClientInterface
        QuickMPCとの通信を担うClient
    _ShareDataFrame__is_result: bool
        send由来のDataFrame(False)なのかexecute由来のDataFrameなのか(True)
    _ShareDataFrame__status: ShareDataFrameStatus
        現在のデータの状態
    """

    __id: str
    __qmpc_request: QMPCRequestInterface
    __is_result: bool = False
    __status: ShareDataFrameStatus = ShareDataFrameStatus.OK

    def _wait_execute(self, progress: bool) -> None:
        """計算が終了するまで待機する

        管理しているIDがjob_uuid(execute由来のID)である場合，計算が終了するまで待機する．
        send_shareしたdata_idを管理している場合は待機する必要がないためすぐにreturnする．

        待機は計算終了まで永遠に行われ，1秒おきにMPCサーバに現在Statusの確認リクエストが送信される．

        Parameters
        ----------
        progress: bool
            計算Statusをログに出力するかどうか

        Returns
        ----------
        None

        Raises
        ------
        QMPCJobError
            計算中に何らかのエラーが発生している時
        """
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
        """テーブルデータを加算する．

        :any:`quickmpc.send_to` で送ったデータでかつ，行数，列数が一致している場合のみ正常に動作する．

        Parameters
        ----------
        other: ShareDataFrame
            加算したいDataFrame

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

        内部ではinner_joinを行う．

        Parameters
        ----------
        other: ShareDataFrame
            結合したいDataFrame
        debug_mode: bool, default=False
            違法な高速結合による高速化をするかどうか

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

        内部ではinner_joinを行う．

        Parameters
        ----------
        others: List[ShareDataFrame]
            結合したいDataFrameのリスト
        debug_mode: bool, default=False
            違法な高速結合による高速化をするかどうか

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
        """列の総和を取得する

        Parameters
        ----------
        columns: list
            計算に用いる列番号(1-index)

        Returns
        ----------
        Result
            結果のDataFrame
        """
        res = self.__qmpc_request.sum([self.__id], columns)
        return ShareDataFrame(res.job_uuid, self.__qmpc_request,
                              True, ShareDataFrameStatus.EXECUTE)

    def mean(self, columns: list) -> "ShareDataFrame":
        """列の平均を取得する

        Parameters
        ----------
        columns: list
            計算に用いる列番号(1-index)

        Returns
        ----------
        Result
            結果のDataFrame
        """
        res = self.__qmpc_request.mean([self.__id], columns)
        return ShareDataFrame(res.job_uuid, self.__qmpc_request,
                              True, ShareDataFrameStatus.EXECUTE)

    def variance(self, columns: list) -> "ShareDataFrame":
        """列の分散を取得する

        Parameters
        ----------
        columns: list
            計算に用いる列番号(1-index)

        Returns
        ----------
        Result
            結果のDataFrame
        """
        res = self.__qmpc_request.variance([self.__id], columns)
        return ShareDataFrame(res.job_uuid, self.__qmpc_request,
                              True, ShareDataFrameStatus.EXECUTE)

    def correl(self, columns1: list, columns2: list) -> "ShareDataFrame":
        """列同士の分散を取得する

        Parameters
        ----------
        columns1: list
            計算に用いる左項の列番号(1-index)
        columns2: list
            計算に用いる右項の列番号(1-index)

        Returns
        ----------
        Result
            結果のDataFrame
        """
        res = self.__qmpc_request.correl([self.__id], columns1, columns2)
        return ShareDataFrame(res.job_uuid, self.__qmpc_request,
                              True, ShareDataFrameStatus.EXECUTE)

    def meshcode(self, columns: list) \
            -> "ShareDataFrame":
        """列のmeshcodeを取得する

        Parameters
        ----------
        columns: list
            計算に用いる列番号(1-index)

        Returns
        ----------
        Result
            結果のDataFrame
        """
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
        None

        Raises
        ------
        RuntimeError
            送信したデータをそのまま保存しようとした場合
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

        Raises
        ------
        RuntimeError
        送信したデータをそのまま取得しようとした場合
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

        Raises
        ------
        RuntimeError
            計算していないデータを指定した場合
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
