import re
from dataclasses import dataclass, field, InitVar
from typing import List, Union

import pandas as pd

import quickmpc.pandas as qpd
from quickmpc.pandas.share_data_frame import ShareDataFrameStatus
from quickmpc.request import QMPCRequest, QMPCRequestInterface
from quickmpc.share import restore
from quickmpc.utils import Dim1, methoddispatch


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

    def send_to(self, df: pd.DataFrame) -> qpd.ShareDataFrame:
        """QuickMPCサーバにデータを送信する．

        `quickmpc.pandas.read_csv()`により読み込んだデータをQuickMPCサーバに送信する．
        `quickmpc.pandas.read_csv()`では``__qmpc_sort_index__``と呼ばれるデータの順序を保持した列がcolumnsに追加されており，send_shareではこの列があることを要求している．
        `quickmpc.pandas.read_csv()`を経由せずにsend_shareする場合は，あらかじめデータ順序を求めて``__qmpc_sort_index__``列を追加しておく必要がある．

        Parameters
        ----------
        df: df.DataFrame
            送信するデータ

        Returns
        ----------
        quickmpc.pandas.ShareDataFrame
            QuickMPC形式のDataframe
        """
        res = self.__qmpc_request.send_share(df, piece_size=1_000_000)
        return qpd.ShareDataFrame(res.data_id, self.__qmpc_request)

    def load_from(self, id_str: str) -> qpd.ShareDataFrame:
        """既に送信してあるデータを参照する．

        Parameters
        ----------
        id_str: str
            既に送信してあるデータのIDか計算で発行したID

        Returns
        ----------
        quickmpc.pandas.ShareDataFrame
            QuickMPC形式のDataframe
        """
        # data_id (256bit hash)
        if re.fullmatch(r'[a-z0-9]{64}', id_str):
            return qpd.ShareDataFrame(id_str, self.__qmpc_request)
        # job_uuid (uuid)
        if re.fullmatch(r'[0-9a-f]{8}-[0-9a-f]{4}-'
                        r'[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{12}', id_str):
            return qpd.ShareDataFrame(id_str, self.__qmpc_request,
                                      True, ShareDataFrameStatus.EXECUTE)
        raise RuntimeError("id must be `data_id` or `job_uuid`.")

    # TODO: job_uuidとparty_sizeは指定しなくても良いようにしたい
    def restore(self, job_uuid: str, filepath: str, party_size: int) \
            -> qpd.ShareDataFrame:
        """既に送信してあるデータを参照する．

        Parameters
        ----------
        job_uuid: str
            データのID
        filepath: str
            dataが保存してあるディレクトリ

        Returns
        ----------
        quickmpc.pandas.ShareDataFrame
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
