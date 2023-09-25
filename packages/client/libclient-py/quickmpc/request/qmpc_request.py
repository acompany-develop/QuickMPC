import csv
import datetime
import hashlib
import json
import logging
import os
import struct
import time
from concurrent.futures import ThreadPoolExecutor
from dataclasses import dataclass, field, InitVar
from typing import Any, Callable, Dict, Iterable, List, Optional, Tuple
from urllib.parse import urlparse

import google.protobuf.json_format
import grpc
import numpy as np
import pandas as pd
import tqdm  # type: ignore
from grpc_status import rpc_status  # type: ignore

import quickmpc.pandas as qpd
from quickmpc.exception import ArgumentError, QMPCJobError, QMPCServerError
from quickmpc.proto.common_types.common_types_pb2 import (ComputationMethod,
                                                          JobErrorInfo, Schema)
from quickmpc.proto.libc_to_manage_pb2 import (AddShareDataFrameRequest,
                                               DeleteSharesRequest,
                                               ExecuteComputationRequest,
                                               GetComputationRequest,
                                               GetComputationResultResponse,
                                               GetElapsedTimeRequest, Input,
                                               JoinOrder, SendSharesRequest)
from quickmpc.proto.libc_to_manage_pb2_grpc import LibcToManageStub
from quickmpc.request.qmpc_request_interface import QMPCRequestInterface
from quickmpc.request.response import (AddShareDataFrameResponse,
                                       ExecuteResponse,
                                       GetComputationStatusResponse,
                                       GetElapsedTimeResponse,
                                       GetJobErrorInfoResponse,
                                       GetResultResponse, SendShareResponse)
from quickmpc.share import Share
from quickmpc.utils import if_present, MakePiece

logger = logging.getLogger(__name__)


def _create_grpc_channel(endpoint: str) -> grpc.Channel:
    """gppcのchannnelを生成する

    入力形式をパースして自動で適切なチャンネルを生成する．
    現在サポートしているのはhttpとhttpsの2つで，
    いずれも `http://~~~` という文字列である必要がある．

    Parameters
    ----------
    endpoint: str
        endpointのIP

    Returns
    --------
    grpc.channel
        grpcのchannel

    Raises
    -------
    ArgumentError
        サポートしていないプロトコルを指定した場合
    """
    channel: grpc.Channel = None
    o = urlparse(endpoint)
    if o.scheme == 'http':
        # insecureなchannelを作成
        channel = grpc.insecure_channel(o.netloc)
    elif o.scheme == 'https':
        # secureなchannelを作成
        credential: grpc.ChannelCredentials \
            = grpc.ssl_channel_credentials()
        channel = grpc.secure_channel(o.netloc, credential)
    else:
        logger.error(f'仕様を満たさない形式のendpointが渡された: {endpoint}')
        raise ArgumentError(
            "endpointsにサポートされてないプロトコルが指定されています．http/httpsのいずれかを指定してください．")

    return channel


@dataclass(frozen=True)
class QMPCRequest(QMPCRequestInterface):
    """QuickMPCサーバと通信を行う

    Args
    ----
    endpoints: List[str]
        各PartyのIP
    _QMPCRequest__retry_num: int
        通信のretry回数
    _QMPCRequest__retry_wait_time : int
        通信のretry待機時間

    Attributes
    ----------
    __client_stubs: Tuple[LibcToManageStub]
        各Partyのgrpcのstub
    __client_channels: Tuple[grpc.Channel]
        各Partyのgrpcのchannnel
    __party_size: int
        Partyの数
    __token: str
        MPCサーバへの通信のtoken
    __retry_num: int
        通信のretry回数
    __retry_wait_time: int
        通信のretry待機時間
    """

    endpoints: InitVar[List[str]]

    __client_stubs: Tuple[LibcToManageStub] = field(init=False)
    __client_channels: Tuple[grpc.Channel] = field(init=False)
    __party_size: int = field(init=False)
    __token: str = field(init=False)
    # TODO: retry manager的なのを作る
    __retry_num: int = 10
    __retry_wait_time: int = 5

    def __post_init__(self, endpoints: List[str]) -> None:
        chs = [_create_grpc_channel(ep) for ep in endpoints]
        stubs = [LibcToManageStub(ch) for ch in chs]
        object.__setattr__(self, "_QMPCRequest__client_channels", chs)
        object.__setattr__(self, "_QMPCRequest__client_stubs", stubs)
        object.__setattr__(self, "_QMPCRequest__party_size", len(endpoints))
        token = os.environ["QMPC_TOKEN"] \
            if "QMPC_TOKEN" in os.environ else "token_demo"
        object.__setattr__(self, "_QMPCRequest__token", token)

    def __retry(self, f: Callable, *request: Any) -> Any:
        """リトライポリシーに従って特定の関数を実行する

        Parameters
        ----------
        f: Callable
            実行するgrpc request
        request: Any
            fに渡す全ての引数

        Returns
        -------
        Any
            f(*request)の実行結果

        Raises
        ------
        QMPCServerError
            MPCサーバへの接続が確立できないなどサーバのエラーが起きた場合
        QMPCJobError
            計算中に何らかのエラーが発生していた場合
        """
        for ch in self.__client_channels:
            # channelの接続チェック
            is_channel_ready = False
            for _ in range(self.__retry_num):
                try:
                    grpc.channel_ready_future(ch).result(
                        self.__retry_wait_time)
                    is_channel_ready = True
                    break
                except grpc.FutureTimeoutError as e:
                    logger.error(e)
            if not is_channel_ready:
                raise QMPCServerError("channel の準備が出来ません")

        for _ in range(self.__retry_num):
            # requestを送る
            try:
                return f(*request)
            except grpc.RpcError as e:
                logger.error(f'{e.details()} ({e.code()})')

                # エラーが詳細な情報を持っているか確認
                status = rpc_status.from_call(e)
                if status is not None:
                    for detail in status.details:
                        if detail.Is(
                            JobErrorInfo.DESCRIPTOR
                        ):
                            # CC で Job 実行時にエラーが発生していた場合
                            # 例外を rethrow する
                            err_info = JobErrorInfo()
                            detail.Unpack(err_info)
                            logger.error(f"job error information: {err_info}")

                            raise QMPCJobError(err_info) from e

                # MC で Internal Server Error が発生している場合
                # 例外を rethrow する
                if e.code() == grpc.StatusCode.UNKNOWN:
                    raise QMPCServerError("backend server return error") from e
            except Exception as e:
                logger.error(e)
            time.sleep(self.__retry_wait_time)
        raise QMPCServerError(f"All {self.__retry_num} times it was an error")

    @staticmethod
    def __futures_result(
            futures: Iterable, enable_progress_bar=True) -> List:
        """進捗ログを出しながらfutureの結果を取得する

        Parameters
        ----------
        futures: concurrent.futures.Executor
            非同期実行している実体
        enable_progress_bar: bool, default=True
            進捗ログを出すかどうか

        Returns
        -------
        List
            各futuresの返り値のリスト

        Raises
        ------
        Exception
            futures実行中の例外
        """
        try:
            if enable_progress_bar:
                futures = tqdm.tqdm(futures, desc='receive')
            response = [f.result() for f in futures]
        except Exception:
            raise
        return response

    def send_share(self, df: pd.DataFrame, piece_size: int = 1_000_000) \
            -> SendShareResponse:
        """ShareをMPCサーバに送信する

        Parameters
        ----------
        df: pd.DataFrame
            送信するデータ
        piece_size: int, default=1_000_000
            分割pieceの各size

        Returns
        -------
        SendShareResponse
            SendSharesRequestの結果

        Raises
        ------
        RuntimeError
           規定されていない引数を与えた場合
        """
        if piece_size < 1000 or piece_size > 1_000_000:
            raise RuntimeError(
                "piece_size must be in the range of 1000 to 1000000")
        if df.isnull().values.sum() != 0:
            raise RuntimeError("規定されたフォーマットでないデータです．")
        sort_index_name = "__qmpc_sort_index__"
        if sort_index_name not in df.columns:
            raise RuntimeError("規定されたフォーマットでないデータです．"
                               "quickmpc.pandas.read_csvで読み込んだデータか，"
                               f"columnsに{sort_index_name}が含まれたデータ"
                               "でなければならないです．")

        # TODO: 無駄に一度tableを再構成しているのでparse関数を書き直す
        df = df.sort_values(by=sort_index_name)
        df = df.drop(sort_index_name, axis=1)
        table = [df.columns.values.tolist()] + \
                [row.tolist() for row in df.values]
        secrets, schema = qpd.parse(table, matching_column=1)

        # pieceに分けてシェア化
        pieces: list = MakePiece.make_pieces(
            secrets, int(piece_size / 10))
        data_id: str = hashlib.sha256(
            str(secrets).encode() + struct.pack('d', time.time())
        ).hexdigest()

        # リクエストパラメータを設定して非同期にリクエスト送信
        sent_at = str(datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S'))
        futures: list = []
        with ThreadPoolExecutor() as executor:
            for piece_id, p in enumerate(tqdm.tqdm(pieces, desc='sharize')):
                shares = Share.sharize(p, self.__party_size)
                for stub, s in zip(self.__client_stubs, shares):
                    req = SendSharesRequest(data_id=data_id,
                                            shares=json.dumps(s),
                                            schema=schema,
                                            piece_id=piece_id,
                                            sent_at=sent_at,
                                            matching_column=1,
                                            token=self.__token)
                    futures.append(executor.submit(self.__retry,
                                                   stub.SendShares,
                                                   req))
        QMPCRequest.__futures_result(futures)
        return SendShareResponse(data_id)

    def __execute_computation(self, method_id: ComputationMethod.ValueType,
                              data_ids: List[str],
                              columns: Tuple[List[int], List[int]],
                              *, debug_mode: bool = False)  \
            -> ExecuteResponse:
        """計算リクエストをMPCサーバに送信する

        MPCサーバ上でJobと呼ばれる計算リクエストを発行する．
        `data_ids` で指定したIDはサーバ内で全てinner_joinされる．

        Parameters
        ----------
        method_id: ComputationMethod.ValueType
            計算の種類を管理するID
        data_ids: List[str]
            計算に用いるデータのID
        columns: Tuple[List[int], List[int]]
            計算に用いるデータの列番号(1-index)
        debug_mode: bool, default=False
            違法な高速結合による高速化をするかどうか

        Returns
        -------
        ExecuteResponse
            計算リクエストの結果
        """
        req = ExecuteComputationRequest(
            method_id=method_id,
            token=self.__token,
            table=JoinOrder(data_ids=data_ids,
                            debug_mode=debug_mode),
            arg=Input(src=columns[0],
                      target=columns[1]),
        )

        # 非同期にリクエスト送信
        with ThreadPoolExecutor() as executor:
            # JobidをMCから貰う関係で単一MC（現在はSP（ID=0）のみ対応）にリクエストを送る
            futures = [
                executor.submit(self.__retry,
                                self.__client_stubs[0].ExecuteComputation,
                                req)]

        response = QMPCRequest.__futures_result(futures)
        return ExecuteResponse(response[0].job_uuid)

    def sum(self, data_ids: List[str], columns: List[int],
            *, debug_mode: bool = False) -> ExecuteResponse:
        """総和計算リクエストをMPCサーバに送信する

        Parameters
        ----------
        data_ids: List[str]
            計算に用いるデータのID
        columns: List[int]
            計算に用いるデータの列番号(1-index)
        debug_mode: bool, default=False
            違法な高速結合による高速化をするかどうか

        Returns
        -------
        ExecuteResponse
            計算リクエストの結果
        """
        return self.__execute_computation(
            ComputationMethod.COMPUTATION_METHOD_SUM,
            data_ids, (columns, []), debug_mode=debug_mode)

    def mean(self, data_ids: List[str], columns: List[int],
             *, debug_mode: bool = False) -> ExecuteResponse:
        """平均計算リクエストをMPCサーバに送信する

        Parameters
        ----------
        data_ids: List[str]
            計算に用いるデータのID
        columns: List[int]
            計算に用いるデータの列番号(1-index)
        debug_mode: bool, default=False
            違法な高速結合による高速化をするかどうか

        Returns
        -------
        ExecuteResponse
            計算リクエストの結果
        """
        return self.__execute_computation(
            ComputationMethod.COMPUTATION_METHOD_MEAN,
            data_ids, (columns, []), debug_mode=debug_mode)

    def variance(self, data_ids: List[str], columns: List[int],
                 *, debug_mode: bool = False) \
            -> ExecuteResponse:
        """分散計算リクエストをMPCサーバに送信する

        Parameters
        ----------
        data_ids: List[str]
            計算に用いるデータのID
        columns: List[int]
            計算に用いるデータの列番号(1-index)
        debug_mode: bool, default=False
            違法な高速結合による高速化をするかどうか

        Returns
        -------
        ExecuteResponse
            計算リクエストの結果
        """
        return self.__execute_computation(
            ComputationMethod.COMPUTATION_METHOD_VARIANCE,
            data_ids, (columns, []), debug_mode=debug_mode)

    def correl(self, data_ids: List[str], inp1: List[int], inp2: List[int],
               *, debug_mode: bool = False) \
            -> ExecuteResponse:
        """相関係数計算リクエストをMPCサーバに送信する

        Parameters
        ----------
        data_ids: List[str]
            計算に用いるデータのID
        inp1: List[int]
            相関係数の左項に該当する列番号(1-index)
        inp2: List[int]
            相関係数の右項に該当する列番号(1-index)
        debug_mode: bool, default=False
            違法な高速結合による高速化をするかどうか

        Returns
        -------
        ExecuteResponse
            計算リクエストの結果
        """
        return self.__execute_computation(
            ComputationMethod.COMPUTATION_METHOD_CORREL,
            data_ids, (inp1, inp2), debug_mode=debug_mode)

    def meshcode(self, data_ids: List[str], inp1: List[int],
                 *, debug_mode: bool = False) \
            -> ExecuteResponse:
        """meshcodeリクエストをMPCサーバに送信する

        Parameters
        ----------
        data_ids: List[str]
            計算に用いるデータのID
        inp1: List[int]
            計算に用いるデータの列番号(1-index)
        debug_mode: bool, default=False
            違法な高速結合による高速化をするかどうか

        Returns
        -------
        ExecuteResponse
            計算リクエストの結果
        """
        return self.__execute_computation(
            ComputationMethod.COMPUTATION_METHOD_MESH_CODE,
            data_ids, (inp1, []), debug_mode=debug_mode)

    def join(self, data_ids: List[str],
             *, debug_mode: bool = False) -> ExecuteResponse:
        """inner_joinリクエストをMPCサーバに送信する

        Parameters
        ----------
        data_ids: List[str]
            計算に用いるデータのID
        debug_mode: bool, default=False
            違法な高速結合による高速化をするかどうか

        Returns
        -------
        ExecuteResponse
            計算リクエストの結果
        """
        return self.__execute_computation(
            ComputationMethod.COMPUTATION_METHOD_JOIN_TABLE,
            data_ids, ([], []), debug_mode=debug_mode)

    @staticmethod
    def __stream_result(stream: Iterable, job_uuid: str, party: int,
                        output_path: Optional[str]) -> Dict:
        """streamから計算結果を取得して結合する

        `output_path` を指定すると計算結果をファイルに書き込み，返り値にデータ本体は含まれない．
        `output_path` がNoneの場合は計算結果はファイルには書き込まれず，返り値に全て含まれる．

        Parameters
        ----------
        stream: Iterable
            grpcのstreamインスタンス
        job_uuid: str
            jobのID
        party: int
            Partyの番号(1-index)
        output_path: Optional[str]
            出力するファイルのパス

        Returns
        -------
        Dict
            streamの結果を結合した辞書
        """
        is_ok: bool = True
        res_list = []
        for res in stream:
            if output_path:
                file_title = "dim1"
                if res.HasField("is_dim2"):
                    file_title = "dim2"
                elif res.HasField("is_schema"):
                    file_title = "schema"

                file_path = f"{output_path }/" + \
                    f"{file_title}-{job_uuid}-{party}-{res.piece_id}.csv"

                with open(file_path, 'w') as f:
                    writer = csv.writer(f)
                    writer.writerow([res.column_number])
                    writer.writerow(res.result)
                res = GetComputationResultResponse(
                    column_number=res.column_number,
                    piece_id=res.piece_id,
                )
            res_list.append(res)
        res_dict: Dict = {"is_ok": is_ok, "responses": res_list}
        return res_dict

    def get_computation_result(self, job_uuid: str,
                               output_path: Optional[str] = None) \
            -> GetResultResponse:
        """計算結果をMPCサーバから取得する

        `output_path` を指定すると計算結果をファイルに書き込み，返り値にデータ本体は含まれない．
        `output_path` がNoneの場合は計算結果はファイルには書き込まれず，返り値に全て含まれる．

        Parameters
        ----------
        job_uuid
            jobのID
        output_path: Optional[str]
            出力するファイルのパス

        Returns
        -------
        GetResultResponse
            計算結果取得リクエストの結果
        """
        # リクエストパラメータを設定
        req = GetComputationRequest(
            job_uuid=job_uuid,
            token=self.__token
        )
        # 非同期にリクエスト送信
        with ThreadPoolExecutor() as executor:
            futures = [executor.submit(self.__retry,
                                       QMPCRequest.__stream_result,
                                       stub.GetComputationResult(req),
                                       job_uuid, party, output_path)
                       for party, stub in enumerate(self.__client_stubs)]
        response = QMPCRequest.__futures_result(futures,
                                                enable_progress_bar=False)
        results_sorted = [sorted(res["responses"], key=lambda r: r.piece_id)
                          for res in response]

        results: Optional[Any] = None
        schema = None
        is_table = False
        if not output_path:
            for res in results_sorted:
                is_dim2 = False
                column_number = 0
                result: Any = []
                schema_1p = []
                for r in res:
                    if r.HasField("is_schema"):
                        if not is_table:
                            is_table = True
                        for val in r.result:
                            col_sch = google.protobuf.json_format.Parse(
                                val, Schema())
                            schema_1p.append(col_sch)
                            # schema = col_sch
                    else:
                        if r.HasField("is_dim2"):
                            is_dim2 = True
                        for val in r.result:
                            result.append(val)

                    column_number = r.column_number

                if is_dim2:
                    result = np.array(result).reshape(-1,
                                                      column_number).tolist()
                if len(schema_1p) != 0:
                    schema = schema_1p
                result = Share.convert_type(result, schema)
                if results is None:
                    results = []
                results.append(result)

        results = if_present(results, Share.recons)
        results = if_present(results, Share.convert_type, schema)
        if is_table and schema:
            columns = [s.name for s in schema]
            return GetResultResponse(pd.DataFrame(results, columns=columns))
        return GetResultResponse(pd.DataFrame(results))

    def get_computation_status(self, job_uuid: str) \
            -> GetComputationStatusResponse:
        """計算ステータスをMPCサーバから取得する

        Parameters
        ----------
        job_uuid: str
            jobのID

        Returns
        -------
        GetComputationStatusResponse
            計算ステータス
        """
        # リクエストパラメータを設定
        req = GetComputationRequest(
            job_uuid=job_uuid,
            token=self.__token
        )
        # 非同期にリクエスト送信
        with ThreadPoolExecutor() as executor:
            futures = [executor.submit(self.__retry,
                                       stub.GetComputationStatus,
                                       req)
                       for stub in self.__client_stubs]
        response = QMPCRequest.__futures_result(futures,
                                                enable_progress_bar=False)
        statuses = [res.status for res in response]
        progresses = [res.progress if res.HasField("progress") else None
                      for res in response]

        return GetComputationStatusResponse(statuses, progresses)

    def get_job_error_info(self, job_uuid: str) -> GetJobErrorInfoResponse:
        """計算のエラー情報をMPCサーバから取得する

        Parameters
        ----------
        job_uuid: str
            jobのID

        Returns
        -------
        GetJobErrorInfoResponse
            計算のエラー情報
        """
        # リクエストパラメータを設定
        req = GetComputationRequest(
            job_uuid=job_uuid,
            token=self.__token
        )
        # 非同期にリクエスト送信
        with ThreadPoolExecutor() as executor:
            futures = [executor.submit(self.__retry, stub.GetJobErrorInfo, req)
                       for stub in self.__client_stubs]
        response = QMPCRequest.__futures_result(
            futures, enable_progress_bar=False)

        job_error_info = [
            res.job_error_info if res.HasField("job_error_info") else None
            for res in response
        ]
        return GetJobErrorInfoResponse(job_error_info)

    def get_elapsed_time(self, job_uuid: str) -> GetElapsedTimeResponse:
        """計算にかかった時間をMPCサーバから取得する

        Parameters
        ----------
        job_uuid: str
            jobのID

        Returns
        -------
        GetElapsedTimeResponse
            計算にかかった時間
        """
        # リクエストパラメータを設定
        req = GetElapsedTimeRequest(
            job_uuid=job_uuid,
            token=self.__token
        )
        # 非同期にリクエスト送信
        with ThreadPoolExecutor() as executor:
            futures = [executor.submit(self.__retry,
                                       stub.GetElapsedTime,
                                       req)
                       for stub in self.__client_stubs]
        response = QMPCRequest.__futures_result(futures)
        elapsed_time = max([res.elapsed_time for res in response])
        return GetElapsedTimeResponse(elapsed_time)

    def delete_share(self, data_ids: List[str]) -> None:
        """送信したShareをMPCサーバから削除する

        Parameters
        ----------
        data_ids: List[str]
            削除したいShareのIDリスト

        Returns
        -------
        None
        """
        req = DeleteSharesRequest(dataIds=data_ids, token=self.__token)
        # 非同期にリクエスト送信
        with ThreadPoolExecutor() as executor:
            futures = [executor.submit(self.__retry, stub.DeleteShares, req)
                       for stub in self.__client_stubs]
        QMPCRequest.__futures_result(futures)

    def add_share_data_frame(self, base_data_id: str, add_data_id: str) \
            -> AddShareDataFrameResponse:
        """テーブル加算リクエストをMPCサーバに送信する

        Parameters
        ----------
        base_data_id: str
            左項のテーブルのID
        add_data_id: str
            右項のテーブルのID

        Returns
        -------
        AddShareDataFrameResponse
            テーブル加算の結果
        """
        req = AddShareDataFrameRequest(base_data_id=base_data_id,
                                       add_data_id=add_data_id,
                                       token=self.__token)
        # 非同期にリクエスト送信
        with ThreadPoolExecutor() as executor:
            futures = [executor.submit(self.__retry,
                                       stub.AddShareDataFrame, req)
                       for stub in self.__client_stubs]
        response = QMPCRequest.__futures_result(futures)
        data_id = response[0].data_id
        return AddShareDataFrameResponse(data_id)
