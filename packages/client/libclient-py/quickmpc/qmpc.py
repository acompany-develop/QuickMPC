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
    __qmpc_request: QMPCRequestInterface
        qmpc serverに対してrequestを送るinterface

    Examples
    --------
    .. code-block:: python3

        # 直接IPを指定する場合
        qmpc = QMPC([
            "http://localhost:50001",
            "http://localhost:50002",
            "http://localhost:50003"
        ])
        # mockなどでrequestクラスを指定する場合
        request = QMPCRequest([~~~])
        qmpc = QMPC(request)
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
        """QuickMPCサーバにデータを送信する

        :doc:`quickmpc.pandas.read_csv` により読み込んだデータをQuickMPCサーバに送信する．
        :doc:`quickmpc.pandas.read_csv` では ``__qmpc_sort_index__`` と呼ばれるデータの順序を保持した列がcolumnsに追加されており，
        このメソッドではこの列があることを要求している．
        :doc:`quickmpc.pandas.read_csv` を経由せずにQuickMPCサーバにデータを送信する場合は，
        あらかじめデータ順序を求めて ``__qmpc_sort_index__`` 列を追加しておく必要がある．

        Parameters
        ----------
        df: pandas.DataFrame
            送信するデータ

        Returns
        ----------
        quickmpc.pandas.ShareDataFrame
            QuickMPC形式のDataframe

        Examples
        --------
        .. code-block:: python3

            # quickmpc.pandas.read_csv で読み込んだデータを送信する
            df = qpd.read_csv("filepath")
            qmpc.send_to(df)

            # 自分で定義したデータを送信する
            qmpc.send_to(df)
            df = pd.DataFrame([[1,2,0], [3,4,1]],
                              columns=["a", "b", "__qmpc_sort_index__"])),

        See Also
        --------
        quickmpc.pandas.read_csv
            quickmpc形式に則る専用のcsv読み取り関数
        """  # noqa #501
        res = self.__qmpc_request.send_share(df, piece_size=1_000_000)
        return qpd.ShareDataFrame(res.data_id, self.__qmpc_request)

    def load_from(self, id_str: str) -> qpd.ShareDataFrame:
        """QuickMPCサーバに送信してあるデータを参照する

        Parameters
        ----------
        id_str: str
            既に送信してあるデータのIDか計算で発行したID

        Returns
        -------
        quickmpc.pandas.ShareDataFrame
            QuickMPC形式のDataframe

        Raises
        ------
        RuntimeError
            形式が異なるIDを指定した場合

        See Also
        --------
        quickmpc.pandas.ShareDataFrame.get_id
            参照するIDを取得するためのメソッド
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
        """ファイルに保存したMPC結果を復元して取得する．

        Parameters
        ----------
        job_uuid: str
            データのID
        filepath: str
            dataが保存してあるディレクトリ
        party_size: int
            MPCのパーティ数

        Returns
        -------
        quickmpc.pandas.ShareDataFrame
            QuickMPC形式のDataframe

        Examples
        --------
        .. code-block:: python3

            # 計算結果をファイルに保存する
            filepath: str # 保存するディレクトリ
            sdf: qpd.ShareDataFrame # 計算結果
            sdf.to_csv(filepath) # 結果を保存

            # 計算結果をファイルから復元する
            job_uuid = sdf.get_id()
            sdf = qmpc.restore(job_uuid, filepath, 3)

        See Also
        --------
        quickmpc.pandas.ShareDataFrame.get_id
            参照するIDを取得するためのメソッド
        quickmpc.pandas.ShareDataFrame.to_csv
            計算結果をファイルに保存するメソッド．restoreはこのメソッドで保存したファイル群に対して適用できる．
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
