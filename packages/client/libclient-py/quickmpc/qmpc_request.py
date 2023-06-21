import csv
import datetime
import hashlib
import json
import logging
import os
import struct
import time
from abc import ABC, abstractmethod
from concurrent.futures import ThreadPoolExecutor
from dataclasses import dataclass, field, InitVar
from typing import Any, Callable, Dict, Iterable, List, Optional, Tuple, Union
from urllib.parse import urlparse

import google.protobuf.json_format
import grpc
import numpy as np
import pandas as pd
import tqdm  # type: ignore
from grpc_status import rpc_status  # type: ignore

from .exception import ArgumentError, QMPCJobError, QMPCServerError
from .proto.common_types.common_types_pb2 import (ComputationMethod,
                                                  JobErrorInfo, JobStatus,
                                                  Schema, ShareValueTypeEnum)
from .proto.libc_to_manage_pb2 import (DeleteSharesRequest,
                                       ExecuteComputationRequest,
                                       GetComputationResultRequest,
                                       GetComputationResultResponse,
                                       GetDataListRequest,
                                       GetElapsedTimeRequest,
                                       GetJobErrorInfoRequest, Input,
                                       JoinOrder, SendSharesRequest)
from .proto.libc_to_manage_pb2_grpc import LibcToManageStub
from .request.qmpc_request_interface import QMPCRequestInterface
from .request.response import (ExecuteResponse, GetJobErrorInfoResponse,
                               GetResultResponse, SendShareResponse)
from .request.status import Status
from .share import Share
from .utils.if_present import if_present
from .utils.make_pieces import MakePiece
from .utils.overload_tools import Dim1, Dim2, Dim3, methoddispatch
from .utils.parse_csv import format_check, parse

logger = logging.getLogger(__name__)


def _create_grpc_channel(endpoint: str) -> grpc.Channel:
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

    Attributes
    ----------
    __endpoints: List[url]
        QuickMPCサーバのURL
    __token: str
        QuickMPCサーバへの通信を担う
    """

    endpoints: InitVar[List[str]]

    __client_stubs: Tuple[LibcToManageStub] = field(init=False)
    __client_channels: Tuple[grpc.Channel] = field(init=False)
    __party_size: int = field(init=False)
    __token: str = "token"
    __retry_num: int = 10
    __retry_wait_time: int = 5

    def __post_init__(self, endpoints: List[str]) -> None:
        chs = [_create_grpc_channel(ep) for ep in endpoints]
        stubs = [LibcToManageStub(ch) for ch in chs]
        object.__setattr__(self, "_QMPCRequest__client_channels", chs)
        object.__setattr__(self, "_QMPCRequest__client_stubs", stubs)
        object.__setattr__(self, "_QMPCRequest__party_size", len(endpoints))

    def __retry(self, f: Callable, *request: Any) -> Any:
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
                raise RuntimeError("channel の準備が出来ません")

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
        raise RuntimeError(f"All {self.__retry_num} times it was an error")

    @staticmethod
    def __futures_result(
            futures: Iterable, enable_progress_bar=True) -> Tuple[bool, List]:
        """ エラーチェックしてfutureのresultを得る """
        is_ok: bool = True
        response: List = []
        try:
            if enable_progress_bar:
                futures = tqdm.tqdm(futures, desc='receive')
            response = [f.result() for f in futures]
        except (QMPCJobError, QMPCServerError):
            raise
        except Exception as e:
            is_ok = False
            logger.error(e)

        return is_ok, response

    @staticmethod
    def __stream_result(stream: Iterable, job_uuid: str, party: int,
                        path: Optional[str]) -> Dict:
        """ エラーチェックしてstreamのresultを得る """
        is_ok: bool = True
        res_list = []
        for res in stream:
            if path is not None:
                file_title = "dim1"
                if res.HasField("is_dim2"):
                    file_title = "dim2"
                elif res.HasField("is_schema"):
                    file_title = "schema"

                file_path = f"{path}/" + \
                    f"{file_title}-{job_uuid}-{party}-{res.piece_id}.csv"

                with open(file_path, 'w') as f:
                    writer = csv.writer(f)
                    writer.writerow([res.column_number])
                    writer.writerow(res.result)
                progress = res.progress if res.HasField('progress') else None
                res = GetComputationResultResponse(
                    column_number=res.column_number,
                    status=res.status,
                    piece_id=res.piece_id,
                    progress=progress,
                )
            res_list.append(res)
        res_dict: Dict = {"is_ok": is_ok, "responses": res_list}
        return res_dict

    def send_share(self, df: pd.DataFrame, piece_size: int = 1_000_000) \
            -> SendShareResponse:
        """ Shareをコンテナに送信 """
        if piece_size < 1000 or piece_size > 1_000_000:
            raise RuntimeError(
                "piece_size must be in the range of 1000 to 1000000")
        if df.isnull().values.sum() != 0:
            raise RuntimeError("規定されたフォーマットでないデータです．")

        # TODO: 無駄に一度tableを再構成しているのでparse関数を書き直す
        table = [["id"] + df.columns.values.tolist()] + \
            [[i] + row.tolist() for i, row in zip(df.index.values, df.values)]
        secrets, schema = parse(table, matching_column=1)

        # TODO: pandasで受け取ってるしpandasの処理でやった方が良さそう
        # NOTE: pandas前処理でmatching_columnは1列目で固定になってる
        sorted_secrets = sorted(secrets, key=lambda row: row[0])
        # pieceに分けてシェア化
        pieces: list = MakePiece.make_pieces(
            sorted_secrets, int(piece_size / 10))
        data_id: str = hashlib.sha256(
            str(sorted_secrets).encode() + struct.pack('d', time.time())
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
        is_ok, _ = QMPCRequest.__futures_result(futures)
        # TODO: __futures_resultの返り値を適切なものに変更する
        if is_ok:
            return SendShareResponse(Status.OK, data_id)
        return SendShareResponse(Status.BadGateway, "")

    def __execute_computation(self, method_id: ComputationMethod.ValueType,
                              data_ids: List[str],
                              inp: Tuple[List, List],
                              *, debug_mode: bool = False)  \
            -> ExecuteResponse:
        """ 計算リクエストを送信 """
        req = ExecuteComputationRequest(
            method_id=method_id,
            token=self.__token,
            table=JoinOrder(data_ids=data_ids,
                            debug_mode=debug_mode),
            arg=Input(src=inp[0],
                      target=inp[1]),
        )

        # 非同期にリクエスト送信
        with ThreadPoolExecutor() as executor:
            # JobidをMCから貰う関係で単一MC（現在はSP（ID=0）のみ対応）にリクエストを送る
            futures = [
                executor.submit(self.__retry,
                                self.__client_stubs[0].ExecuteComputation,
                                req)]

        is_ok, response = QMPCRequest.__futures_result(futures)
        # TODO: __futures_resultの返り値を適切なものに変更する
        if is_ok:
            return ExecuteResponse(Status.OK, response[0].job_uuid)
        return ExecuteResponse(Status.BadGateway, "")

    def sum(self, data_ids: List[str], inp: List[int]) -> ExecuteResponse:
        return self.__execute_computation(
            ComputationMethod.COMPUTATION_METHOD_SUM,
            data_ids, (inp, []))

    def mean(self, data_ids: List[str], inp: List[int]) -> ExecuteResponse:
        return self.__execute_computation(
            ComputationMethod.COMPUTATION_METHOD_MEAN,
            data_ids, (inp, []))

    def variance(self, data_ids: List[str], inp: List[int]) \
            -> ExecuteResponse:
        return self.__execute_computation(
            ComputationMethod.COMPUTATION_METHOD_VARIANCE,
            data_ids, (inp, []))

    def correl(self, data_ids: List[str], inp1: List[int], inp2: List[int]) \
            -> ExecuteResponse:
        return self.__execute_computation(
            ComputationMethod.COMPUTATION_METHOD_CORREL,
            data_ids, (inp1, inp2))

    def meshcode(self, data_ids: List[str], inp1: List[int], inp2: List[int]) \
            -> ExecuteResponse:
        return self.__execute_computation(
            ComputationMethod.COMPUTATION_METHOD_MESH_CODE,
            data_ids, (inp1, inp2))

    def join(self, data_ids: List[str]) -> ExecuteResponse:
        return self.__execute_computation(
            ComputationMethod.COMPUTATION_METHOD_JOIN_TABLE,
            data_ids, ([], []))

    def get_computation_result(self, job_uuid: str, filepath: str) \
        -> GetResultResponse: ...

    def get_job_error_info(self, job_uuid: str) -> GetJobErrorInfoResponse: ...
