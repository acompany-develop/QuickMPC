from dataclasses import dataclass, field, InitVar
from typing import List, Union

import numpy as np
import pandas as pd

from .qmpc_request import QMPCRequest
from .request.qmpc_request_interface import QMPCRequestInterface
from .share_data_frame import ShareDataFrame
from .utils.overload_tools import Dim1, methoddispatch
from .utils.parse_csv import to_float


@dataclass(frozen=True)
class QMPC:
    """QuickMPCのdataのやり取りをするクラス

    Attributes
    ----------
    arg: Union[List[str], QMPCRequestInterface]]
        parties: List[str]
            serverのIP
        qmpc_request: QMPCRequestMock
            qmpc serverに対してrequestを送るinterface
    """

    arg: InitVar[Union[List[str], QMPCRequestInterface]]

    __qmpc_request: QMPCRequestInterface = field(init=False)

    @methoddispatch()
    def __post_init__(self, *args, **kwargs):
        raise NotImplementedError("QMPCClientにサポートしていない値が与えられた．")

    @__post_init__.register(Dim1)
    def __post_init__default(self, parties: List[str]):
        object.__setattr__(self, "_QMPC__qmpc_request",
                           QMPCRequest(parties))

    @__post_init__.register(QMPCRequest)
    def __post_init__original(self, qmpc_request: QMPCRequest):
        # mydispatchを改造してinterfaceでオーバーロードさせる
        object.__setattr__(self, "_QMPC__qmpc_request", qmpc_request)

    def read_csv(self, *args, index_col: str, **kwargs) -> pd.DataFrame:
        """csvからテーブルデータを読み込む．

        テーブル結合処理に用いる列がどの列かを`index_col`で指定する必要がある．
        `index_col`以外の引数は全てpandasのread_csvと同じ．

        Parameters
        ----------
        filepath_or_buffer: str, path object or file-like object
            らしい
        index_col: str
            ID列としたいカラム名

        Returns
        ----------
        pd.DataFrame
            読み込んだテーブルデータ
        """
        df = pd.read_csv(*args, **kwargs)
        # ID列を数値化
        df[index_col] = df[index_col].map(lambda x: to_float(x))
        # join時にQMPCのCC側でID列でsortできる様に、座圧を行いindexに設定しておく
        df["original_index"] = df.index
        df = df.sort_values(by=index_col)
        df = df.reset_index(drop=True)
        df = df.sort_values(by="original_index")
        df = df.drop('original_index', axis=1)
        df.set_index(index_col)
        return df

    def send_to(self, df: pd.DataFrame) -> ShareDataFrame:
        """QuickMPCサーバにデータを送信する．

        Parameters
        ----------
        df: df.DataFrame
            送信するデータ

        Returns
        ----------
        ShareDataFrame
            QuickMPC形式のDataframe
        """
        # send_shareできる形式に変換
        res = self.__qmpc_request.send_share(df, piece_size=1_000_000)
        return ShareDataFrame(res.data_id, self.__qmpc_request)

    def load_from(self, data_id: str) -> ShareDataFrame:
        """既に送信してあるデータを参照する．

        Parameters
        ----------
        data_id: str
            既に送信してあるデータのID

        Returns
        ----------
        ShareDataFrame
            QuickMPC形式のDataframe
        """
        return ShareDataFrame(data_id, self.__qmpc_request)
