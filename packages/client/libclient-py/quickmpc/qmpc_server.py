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
from typing import Any, Callable, Dict, Iterable, List, Optional, Tuple, Union
from urllib.parse import urlparse

import google.protobuf.json_format
import grpc
import numpy as np
import tqdm  # type: ignore
from grpc_status import rpc_status  # type: ignore

from .exception import ArgumentError, QMPCJobError, QMPCServerError
from .proto.common_types.common_types_pb2 import (JobErrorInfo, JobStatus,
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
from .share import Share
from .utils.if_present import if_present
from .utils.make_pieces import MakePiece
from .utils.overload_tools import Dim1, Dim2, Dim3, methoddispatch
from .utils.parse_csv import format_check

abs_file = os.path.abspath(__file__)
base_dir = os.path.dirname(abs_file)

logger = logging.getLogger(__name__)


@dataclass(frozen=True)
class QMPCServer:
    endpoints: InitVar[List[str]]
    __client_stubs: Tuple[LibcToManageStub] = field(init=False)
    __client_channels: Tuple[grpc.Channel] = field(init=False)
    __party_size: int = field(init=False)
    token: str
    retry_num: int = 10
    retry_wait_time: int = 5

    def __post_init__(self, endpoints: List[str]) -> None:
        chs = [QMPCServer.__create_grpc_channel(ep) for ep in endpoints]
        stubs = [LibcToManageStub(ch) for ch in chs]
        object.__setattr__(self, "_QMPCServer__client_channels", chs)
        object.__setattr__(self, "_QMPCServer__client_stubs", stubs)
        object.__setattr__(self, "_QMPCServer__party_size", len(endpoints))

    @staticmethod
    def __create_grpc_channel(endpoint: str) -> grpc.Channel:
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

    @staticmethod
    def _argument_check(join_order: Tuple[List, List, List]):
        if len(join_order[0]) - 1 != len(join_order[1]):
            logger.error(
                'the size of join must be one less than the size of dataIds')
            return False
        if len(join_order[0]) != len(join_order[2]):
            logger.error('the size of index must match the size of dataIds')
            return False
        # TODO joinをenumにする
        if not all([0 <= join <= 2 for join in join_order[1]]):
            logger.error('join value must be in the range of 0 to 2')
            return False
        return True

    def __retry(self, f: Callable, *request: Any) -> Any:
        for ch in self.__client_channels:
            # channelの接続チェック
            is_channel_ready = False
            for _ in range(self.retry_num):
                try:
                    grpc.channel_ready_future(ch).result(self.retry_wait_time)
                    is_channel_ready = True
                    break
                except grpc.FutureTimeoutError as e:
                    logger.error(e)
            if not is_channel_ready:
                raise RuntimeError("channel の準備が出来ません")

        for _ in range(self.retry_num):
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
            time.sleep(self.retry_wait_time)
        raise RuntimeError(f"All {self.retry_num} times it was an error")

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

    @methoddispatch(is_static_method=True)
    @staticmethod
    def __convert_schema(_):
        raise ArgumentError("不正な引数が与えられています．")

    @__convert_schema.register(Dim1)
    @staticmethod
    def __convert_schema_dummy(schema: List):
        raise ArgumentError("不正な引数が与えられています．")

    @__convert_schema.register((Dim1, str))
    @staticmethod
    def __convert_schema_str(schema: List[str]) -> List[Schema]:
        return [
            Schema(name=name,
                   type=ShareValueTypeEnum.SHARE_VALUE_TYPE_FIXED_POINT)
            for name in schema]

    @__convert_schema.register((Dim1, Schema))
    @staticmethod
    def __convert_schema_typed(
            schema: List[Schema]) -> List[Schema]:
        return schema

    @methoddispatch()
    def send_share(self, _):
        raise ArgumentError("不正な引数が与えられています．")

    @send_share.register(Dim2)
    @send_share.register(Dim3)
    def __send_share_impl(self, secrets: List,
                          schema: List[Union[str, Schema]],
                          matching_column: int,
                          piece_size: int) -> Dict:
        if piece_size < 1000 or piece_size > 1_000_000:
            raise RuntimeError(
                "piece_size must be in the range of 1000 to 1000000")

        if matching_column <= 0 or matching_column > len(schema):
            raise RuntimeError(
                "matching_column must be in the "
                "range of 1 to the size of schema")

        typed_schema: List[Schema] = QMPCServer.__convert_schema(schema)

        # TODO parse_csv経由でsend_shareをすると同じチェックをすることになる．
        if not format_check(secrets, schema):
            raise RuntimeError("規定されたフォーマットでないデータです．")

        """ Shareをコンテナに送信 """
        sorted_secrets = sorted(
            secrets, key=lambda row: row[matching_column - 1])
        # pieceに分けてシェア化
        pieces: list = MakePiece.make_pieces(
            sorted_secrets, int(piece_size / 10))
        data_id: str = hashlib.sha256(
            str(sorted_secrets).encode() + struct.pack('d', time.time())
        ).hexdigest()
        shares = [Share.sharize(s, self.__party_size)
                  for s in tqdm.tqdm(pieces, desc='sharize')]
        sent_at = str(datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S'))

        # リクエストパラメータを設定して非同期にリクエスト送信
        with ThreadPoolExecutor() as executor:
            futures = [executor.submit(self.__retry,
                                       stub.SendShares,
                                       SendSharesRequest(
                                           data_id=data_id,
                                           shares=json.dumps(s),
                                           schema=typed_schema,
                                           piece_id=piece_id,
                                           sent_at=sent_at,
                                           matching_column=matching_column,
                                           token=self.token))
                       for piece_id, share_piece in enumerate(shares)
                       for stub, s in zip(self.__client_stubs, share_piece)
                       ]
        is_ok, _ = QMPCServer.__futures_result(futures)
        return {"is_ok": is_ok, "data_id": data_id}

    def delete_share(self, data_ids: List[str]) -> Dict:
        """ Shareを削除 """
        req = DeleteSharesRequest(dataIds=data_ids, token=self.token)
        # 非同期にリクエスト送信
        with ThreadPoolExecutor() as executor:
            futures = [executor.submit(self.__retry, stub.DeleteShares, req)
                       for stub in self.__client_stubs]
        is_ok, _ = QMPCServer.__futures_result(futures)
        return {"is_ok": is_ok}

    def execute_computation(self, method_id: int,
                            join_order: Tuple[List, List, List],
                            inp: Tuple[List, List]) -> Dict:
        if not self._argument_check(join_order):
            raise ArgumentError("引数が正しくありません．")

        """ 計算リクエストを送信 """
        join_order_req = JoinOrder(
            dataIds=join_order[0],
            join=join_order[1],
            index=join_order[2])
        input_req = Input(
            src=inp[0],
            target=inp[1])
        req = ExecuteComputationRequest(
            method_id=method_id,
            token=self.token,
            table=join_order_req,
            arg=input_req,
        )

        # 非同期にリクエスト送信
        with ThreadPoolExecutor() as executor:
            # JobidをMCから貰う関係で単一MC（現在はSP（ID=0）のみ対応）にリクエストを送る
            futures = [
                executor.submit(self.__retry,
                                self.__client_stubs[0].ExecuteComputation,
                                req)]

        is_ok, response = QMPCServer.__futures_result(futures)
        job_uuid = response[0].job_uuid if is_ok else None

        return {"is_ok": is_ok, "job_uuid": job_uuid}

    def get_data_list(self) -> Dict:
        # 非同期にリクエスト送信
        with ThreadPoolExecutor() as executor:
            futures = [executor.submit(self.__retry,
                                       stub.GetDataList,
                                       GetDataListRequest(token=self.token))
                       for stub in self.__client_stubs]
        is_ok, response = QMPCServer.__futures_result(futures)
        results = [eval(r.result) for r in response] if is_ok else None

        return {"is_ok": is_ok, "results": results}

    def get_elapsed_time(self, job_uuid: str) -> Dict:
        # リクエストパラメータを設定
        req = GetElapsedTimeRequest(
            job_uuid=job_uuid,
            token=self.token
        )
        # 非同期にリクエスト送信
        with ThreadPoolExecutor() as executor:
            futures = [executor.submit(self.__retry,
                                       stub.GetElapsedTime,
                                       req)
                       for stub in self.__client_stubs]
        is_ok, response = QMPCServer.__futures_result(futures)
        elapsed_time = max([res.elapsed_time
                            for res in response]) if is_ok else None
        return {"is_ok": is_ok, "elapsed_time": elapsed_time}

    def get_computation_result(self, job_uuid: str,
                               path: Optional[str]) -> Dict:
        """ コンテナから結果を取得 """
        # リクエストパラメータを設定
        req = GetComputationResultRequest(
            job_uuid=job_uuid,
            token=self.token
        )
        # 非同期にリクエスト送信
        with ThreadPoolExecutor() as executor:
            futures = [executor.submit(self.__retry,
                                       QMPCServer.__stream_result,
                                       stub.GetComputationResult(req),
                                       job_uuid, party, path)
                       for party, stub in enumerate(self.__client_stubs)]
        is_ok, response = QMPCServer.__futures_result(
            futures, enable_progress_bar=False)
        results_sorted = [sorted(res["responses"], key=lambda r: r.piece_id)
                          for res in response]
        # NOTE: statusは0番目(piece_id=1)の要素にのみ含まれている
        statuses = [res[0].status for res in results_sorted] \
            if results_sorted else None
        all_completed = all([
            s == JobStatus.Value('COMPLETED') for s in statuses
        ]) if statuses is not None else False

        progresses = None
        if results_sorted is not None:
            progresses = [
                res[0].progress if res[0].HasField("progress") else None
                for res in results_sorted
            ]

        results: Optional[Any] = None
        schema = None
        is_table = False
        if not path and all_completed:
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
        if is_table:
            results = {"schema": schema, "table": results}
        return {"is_ok": is_ok, "statuses": statuses,
                "results": results, "progresses": progresses}

    def get_job_error_info(self, job_uuid: str) -> Dict:
        # リクエストパラメータを設定
        req = GetJobErrorInfoRequest(
            job_uuid=job_uuid,
            token=self.token
        )
        # 非同期にリクエスト送信
        with ThreadPoolExecutor() as executor:
            futures = [executor.submit(self.__retry, stub.GetJobErrorInfo, req)
                       for stub in self.__client_stubs]
        is_ok, response = QMPCServer.__futures_result(
            futures, enable_progress_bar=False)

        job_error_info = [
            res.job_error_info if res.HasField("job_error_info") else None
            for res in response
        ]

        return {"is_ok": is_ok, "job_error_info": job_error_info}
