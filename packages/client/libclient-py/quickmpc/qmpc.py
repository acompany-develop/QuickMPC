import logging
from dataclasses import dataclass, field, InitVar
from typing import Dict, List, Optional, Tuple

from .proto.common_types import common_types_pb2
from .qmpc_server import QMPCServer
from .share import Share
from .utils.parse_csv import parse, parse_csv
from .restore import restore

logger = logging.getLogger(__name__)
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
    # tokenがちゃんと使用されるようになったらデフォルト値を外す
    token: InitVar[str] = "token_demo"
    retry_num: InitVar[int] = 10
    retry_wait_time: InitVar[int] = 5

    __qmpc_server: QMPCServer = field(init=False)
    __party_size: int = field(init=False)

    def __post_init__(self, endpoints: List[str],
                      token: str, retry_num: int, retry_wait_time: int):
        logger.info(f"[QuickMPC server IP]={endpoints}")
        object.__setattr__(self, "_QMPC__qmpc_server", QMPCServer(
            endpoints, token, retry_num, retry_wait_time))
        object.__setattr__(self, "_QMPC__party_size", len(endpoints))

    def send_share_from_csv_file(self,
                                 filename: str,
                                 matching_column: int = 1,
                                 piece_size: int = 1_000_000) -> Dict:
        secrets, schema = parse_csv(filename, matching_column)
        logger.info("send_share_from_csv_file. "
                    f"[filename]='{filename}'"
                    f"[matching ID name]={schema[matching_column-1]}")
        return self.__qmpc_server.send_share(
            secrets, schema, matching_column, piece_size)

    def send_share_from_csv_data(self,
                                 data: List[List[str]],
                                 matching_column: int = 1,
                                 piece_size: int = 1_000_000) -> Dict:
        secrets, schema = parse(data, matching_column)
        logger.info("send_share_from_csv_data. "
                    f"[secrets size]={len(secrets)}x{len(secrets[0])} "
                    f"[matching ID name]={schema[matching_column-1]}")
        return self.__qmpc_server.send_share(
            secrets, schema, matching_column, piece_size)

    def delete_share(self, data_ids: List[str]) -> Dict:
        logger.info("delete_share request. "
                    f"[delete id list]={data_ids}")
        return self.__qmpc_server.delete_share(data_ids)

    def mean(self, data_ids: List[str], src: List,
             *, debug_mode: bool = False) -> Dict:
        logger.info("mean request. "
                    f"[data_id list]={data_ids} "
                    f"[src columns]={src} "
                    f"[debug_mode]={debug_mode}")
        return self.__qmpc_server.execute_computation(
            ComputationMethod.Value("COMPUTATION_METHOD_MEAN"),
            data_ids, (src, []), debug_mode=debug_mode)

    def variance(self, data_ids: List[str], src: List,
                 *, debug_mode: bool = False) -> Dict:
        logger.info("variance request. "
                    f"[data_id list]={data_ids} "
                    f"[src columns]={src} "
                    f"[debug_mode]={debug_mode}")
        return self.__qmpc_server.execute_computation(
            ComputationMethod.Value("COMPUTATION_METHOD_VARIANCE"),
            data_ids, (src, []), debug_mode=debug_mode)

    def sum(self, data_ids: List[str], src: List,
            *, debug_mode: bool = False) -> Dict:
        logger.info("sum request. "
                    f"[data_id list]={data_ids} "
                    f"[src columns]={src} "
                    f"[debug_mode]={debug_mode}")
        return self.__qmpc_server.execute_computation(
            ComputationMethod.Value("COMPUTATION_METHOD_SUM"),
            data_ids, (src, []), debug_mode=debug_mode)

    def correl(self, data_ids: List[str], inp: Tuple[List[int], List[int]],
               *, debug_mode: bool = False) -> Dict:
        logger.info("correl request. "
                    f"[data_id list]={data_ids} "
                    f"[src columns]={inp[0]}"
                    f"[target columns]={inp[1]} "
                    f"[debug_mode]={debug_mode}")
        return self.__qmpc_server.execute_computation(
            ComputationMethod.Value("COMPUTATION_METHOD_CORREL"),
            data_ids, inp, debug_mode=debug_mode)

    def meshcode(self, data_ids: List[str], src: List,
                 *, debug_mode: bool = False) -> Dict:
        logger.info("meshcode request. "
                    f"[data_id list]={data_ids} "
                    f"[src columns]={src} "
                    f"[debug_mode]={debug_mode}")
        return self.__qmpc_server.execute_computation(
            ComputationMethod.Value("COMPUTATION_METHOD_MESH_CODE"),
            data_ids, (src, []), debug_mode=debug_mode)

    def get_join_table(self, data_ids: List[str],
                       *, debug_mode: bool = False) -> Dict:
        logger.info("get_join_table request. "
                    f"[data_id list]={data_ids} "
                    f"[debug_mode]={debug_mode}")
        return self.__qmpc_server.execute_computation(
            ComputationMethod.Value("COMPUTATION_METHOD_JOIN_TABLE"),
            data_ids, ([], []), debug_mode=debug_mode)

    def get_computation_result(self, job_uuid: str,
                               path: Optional[str] = None) -> Dict:
        logger.info("get_computation_result request. "
                    f"[job_uuid]={job_uuid} "
                    f"[path]={path}")
        return self.__qmpc_server.get_computation_result(job_uuid, path)

    def get_data_list(self) -> Dict:
        logger.info("get_data_list request.")
        return self.__qmpc_server.get_data_list()

    def demo_sharize(self, secrets: List) -> Dict:
        logger.info("demo_sharize request. "
                    f"[secrets size]={len(secrets)}x{len(secrets[0])}")
        share = Share.sharize(secrets, self.__party_size)
        return {'is_ok': True, 'results': share}

    def get_elapsed_time(self, job_uuid: str) -> Dict:
        logger.info("get_elapsed_time request. "
                    f"[job_uuid]={job_uuid}")
        return self.__qmpc_server.get_elapsed_time(job_uuid)

    def restore(self, job_uuid: str, path: str):
        logger.info("restore request. "
                    f"[job_uuid]={job_uuid} "
                    f"[path]={path}")
        return restore(job_uuid, path, self.__party_size)

    def get_job_error_info(self, job_uuid: str) -> Dict:
        logger.info("get_job_error_info request. "
                    f"[job_uuid]={job_uuid}")
        return self.__qmpc_server.get_job_error_info(job_uuid)

    @staticmethod
    def set_log_level(level: int):
        logger.setLevel(level)
