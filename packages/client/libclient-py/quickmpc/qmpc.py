import io
from dataclasses import dataclass, field, InitVar
from typing import Dict, List, Optional, Tuple, Union

import pandas as pd

from .proto.common_types import common_types_pb2
from .qmpc_logging import get_logger
from .qmpc_request import QMPCRequest
from .request.status import Status
from .restore import restore
from .share import Share
from .utils.parse_csv import parse, parse_csv, to_float

logger = get_logger()
# qmpc.JobStatus でアクセスできるようにエイリアスを設定する
JobStatus \
    = common_types_pb2.JobStatus
ComputationMethod \
    = common_types_pb2.ComputationMethod
JobErrorInfo = common_types_pb2.JobErrorInfo
Schema = common_types_pb2.Schema
ShareValueTypeEnum = common_types_pb2.ShareValueTypeEnum


@dataclass(frozen=True)
class QMPC:
    endpoints: InitVar[List[str]]
    retry_num: InitVar[int] = 10
    retry_wait_time: InitVar[int] = 5

    __qmpc_request: QMPCRequest = field(init=False)
    __party_size: int = field(init=False)

    def __post_init__(self, endpoints: List[str],
                      retry_num: int, retry_wait_time: int):
        logger.info(f"[QuickMPC server IP]={endpoints}")
        object.__setattr__(self, "_QMPC__qmpc_request", QMPCRequest(
            endpoints, retry_num, retry_wait_time))
        object.__setattr__(self, "_QMPC__party_size", len(endpoints))

    def send_share_from_df(self,
                           df: pd.DataFrame,
                           matching_column: int = 1,
                           piece_size: int = 1_000_000) -> Dict:
        index_col = df.columns[matching_column-1]
        logger.info("send_share. "
                    f"[matching ID name]={index_col}")
        # ID列を1列目に持ってくる
        df = df.iloc[:, [matching_column-1] +
                     [i for i in range(len(df.columns))
                      if i != matching_column-1]]
        # ID列を数値化
        df[index_col] = df[index_col].map(lambda x: to_float(x))
        # join時にQMPCのCC側でID列でsortできる様に、座圧を行いindexに設定しておく
        df["original_index"] = df.index
        df = df.sort_values(by=index_col) \
            .reset_index(drop=True) \
            .sort_values(by="original_index") \
            .drop('original_index', axis=1)
        res = self.__qmpc_request.send_share(df, piece_size=piece_size)
        return {"is_ok": res.status == Status.OK, "data_id": res.data_id}

    def send_share_from_csv_file(self,
                                 filename: Union[str, io.StringIO],
                                 matching_column: int = 1,
                                 piece_size: int = 1_000_000) -> Dict:
        df = pd.read_csv(filename)
        return self.send_share_from_df(df, matching_column, piece_size)

    def send_share_from_csv_data(self,
                                 data: List[List[str]],
                                 matching_column: int = 1,
                                 piece_size: int = 1_000_000) -> Dict:
        df = pd.DataFrame(data[1:], columns=data[0])
        return self.send_share_from_df(df, matching_column, piece_size)

    def delete_share(self, data_ids: List[str]) -> Dict:
        logger.info("delete_share request. "
                    f"[delete id list]={data_ids}")
        return self.__qmpc_request.delete_share(data_ids)

    def mean(self, data_ids: List[str], src: List,
             *, debug_mode: bool = False) -> Dict:
        logger.info("mean request. "
                    f"[data_id list]={data_ids} "
                    f"[src columns]={src} "
                    f"[debug_mode]={debug_mode}")
        res = self.__qmpc_request.mean(data_ids, src, debug_mode=debug_mode)
        return {"is_ok": res.status == Status.OK, "job_uuid": res.job_uuid}

    def variance(self, data_ids: List[str], src: List,
                 *, debug_mode: bool = False) -> Dict:
        logger.info("variance request. "
                    f"[data_id list]={data_ids} "
                    f"[src columns]={src} "
                    f"[debug_mode]={debug_mode}")
        res = self.__qmpc_request.variance(
            data_ids, src, debug_mode=debug_mode)
        return {"is_ok": res.status == Status.OK, "job_uuid": res.job_uuid}

    def sum(self, data_ids: List[str], src: List,
            *, debug_mode: bool = False) -> Dict:
        logger.info("sum request. "
                    f"[data_id list]={data_ids} "
                    f"[src columns]={src} "
                    f"[debug_mode]={debug_mode}")
        res = self.__qmpc_request.sum(data_ids, src, debug_mode=debug_mode)
        return {"is_ok": res.status == Status.OK, "job_uuid": res.job_uuid}

    def correl(self, data_ids: List[str], inp: Tuple[List[int], List[int]],
               *, debug_mode: bool = False) -> Dict:
        logger.info("correl request. "
                    f"[data_id list]={data_ids} "
                    f"[src columns]={inp[0]}"
                    f"[target columns]={inp[1]} "
                    f"[debug_mode]={debug_mode}")
        res = self.__qmpc_request.correl(
            data_ids, inp[0], inp[1], debug_mode=debug_mode)
        return {"is_ok": res.status == Status.OK, "job_uuid": res.job_uuid}

    def meshcode(self, data_ids: List[str], src: List,
                 *, debug_mode: bool = False) -> Dict:
        logger.info("meshcode request. "
                    f"[data_id list]={data_ids} "
                    f"[src columns]={src} "
                    f"[debug_mode]={debug_mode}")
        res = self.__qmpc_request.meshcode(
            data_ids, src, debug_mode=debug_mode)
        return {"is_ok": res.status == Status.OK, "job_uuid": res.job_uuid}

    def get_join_table(self, data_ids: List[str],
                       *, debug_mode: bool = False) -> Dict:
        logger.info("get_join_table request. "
                    f"[data_id list]={data_ids} "
                    f"[debug_mode]={debug_mode}")
        res = self.__qmpc_request.join(data_ids, debug_mode=debug_mode)
        return {"is_ok": res.status == Status.OK, "job_uuid": res.job_uuid}

    def get_computation_result(self, job_uuid: str,
                               path: Optional[str] = None) -> Dict:
        logger.info("get_computation_result request. "
                    f"[job_uuid]={job_uuid} "
                    f"[path]={path}")
        res = self.__qmpc_request.get_computation_result(job_uuid,
                                                         output_path=path)
        return {"is_ok": res.status == Status.OK, "statuses": res.job_statuses,
                "results": res.results, "progresses": res.progresses}

    def get_data_list(self) -> Dict:
        logger.info("get_data_list request.")
        return self.__qmpc_request.get_data_list()

    def demo_sharize(self, secrets: List) -> Dict:
        logger.info("demo_sharize request. "
                    f"[secrets size]={len(secrets)}x{len(secrets[0])}")
        share = Share.sharize(secrets, self.__party_size)
        return {'is_ok': True, 'results': share}

    def get_elapsed_time(self, job_uuid: str) -> Dict:
        logger.info("get_elapsed_time request. "
                    f"[job_uuid]={job_uuid}")
        return self.__qmpc_request.get_elapsed_time(job_uuid)

    def restore(self, job_uuid: str, path: str):
        logger.info("restore request. "
                    f"[job_uuid]={job_uuid} "
                    f"[path]={path}")
        return restore(job_uuid, path, self.__party_size)

    def get_job_error_info(self, job_uuid: str) -> Dict:
        logger.info("get_job_error_info request. "
                    f"[job_uuid]={job_uuid}")
        res = self.__qmpc_request.get_job_error_info(job_uuid)
        return {"is_ok": res.status == Status.OK,
                "job_error_info": res.job_error_info}

    @staticmethod
    def set_log_level(level: int):
        logger.setLevel(level)
