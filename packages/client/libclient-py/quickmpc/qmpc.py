from dataclasses import dataclass, field, InitVar
from typing import List, Union

import pandas as pd

from .pandas.share_data_frame import ShareDataFrame
from .qmpc_request import QMPCRequest
from .request.qmpc_request_interface import QMPCRequestInterface
from .restore import restore
from .utils.overload_tools import Dim1, methoddispatch


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

    # TODO: job_uuidとparty_sizeは指定しなくても良いようにしたい
    def restore(self, job_uuid: str, filepath: str, party_size: int) \
            -> ShareDataFrame:
        """既に送信してあるデータを参照する．

        Parameters
        ----------
        job_uuid: str
            データのID
        filepath: str
            dataが保存してあるディレクトリ

        Returns
        ----------
        ShareDataFrame
            QuickMPC形式のDataframe
        """
        # TODO: get_computation_resultと同じ処理なのでうまくまとめる
        res = restore(job_uuid, filepath, party_size)
        if res is None:
            return pd.DataFrame()
        if type(res) == dict:
            if res["schema"] is None or res["table"] is None:
                return pd.DataFrame()
            schema = [s.name for s in res["schema"]]
            df = pd.DataFrame(res["table"],
                              columns=schema)
            return df
        return pd.DataFrame(res)
