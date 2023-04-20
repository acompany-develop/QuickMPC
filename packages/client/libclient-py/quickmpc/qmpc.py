import logging
from dataclasses import dataclass, field, InitVar
from typing import Dict, List, Optional, Tuple

from .proto.common_types import common_types_pb2
from .qmpc_server import QMPCServer
from .share import Share
from .utils.parse_csv import (parse, parse_csv)
from .utils.restore import restore

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

    __qmpc_server: QMPCServer = field(init=False)
    __party_size: int = field(init=False)

    def __post_init__(self, endpoints: List[str],
                      token: str):
        logger.info(f"[QuickMPC server IP]={endpoints}")
        object.__setattr__(self, "_QMPC__qmpc_server", QMPCServer(
            endpoints, token))
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

    def mean(self, join_order: Tuple[List[str], List[int], List[int]],
             src: List) -> Dict:
        logger.info("mean request. "
                    f"[data_id list]={join_order[0]} "
                    f"[join method]={join_order[1]} "
                    f"[matching ID columns]={join_order[2]} "
                    f"[src columns]={src}")
        return self.__qmpc_server.execute_computation(
            ComputationMethod.Value("COMPUTATION_METHOD_MEAN"),
            join_order, (src, []))

    def variance(self, join_order: Tuple[List[str], List[int], List[int]],
                 src: List) -> Dict:
        logger.info("variance request. "
                    f"[data_id list]={join_order[0]} "
                    f"[join method]={join_order[1]} "
                    f"[matching ID columns]={join_order[2]} "
                    f"[src columns]={src}")
        return self.__qmpc_server.execute_computation(
            ComputationMethod.Value("COMPUTATION_METHOD_VARIANCE"),
            join_order, (src, []))

    def sum(self, join_order: Tuple[List[str], List[int], List[int]],
            src: List) -> Dict:
        logger.info("sum request. "
                    f"[data_id list]={join_order[0]} "
                    f"[join method]={join_order[1]} "
                    f"[matching ID columns]={join_order[2]} "
                    f"[src columns]={src}")
        return self.__qmpc_server.execute_computation(
            ComputationMethod.Value("COMPUTATION_METHOD_SUM"),
            join_order, (src, []))

    def correl(self, join_order: Tuple[List[str], List[int], List[int]],
               inp: Tuple[List[int], List[int]]) -> Dict:
        logger.info("correl request. "
                    f"[data_id list]={join_order[0]} "
                    f"[join method]={join_order[1]} "
                    f"[matching ID columns]={join_order[2]} "
                    f"[src columns]={inp[0]}"
                    f"[target columns]={inp[1]}")
        return self.__qmpc_server.execute_computation(
            ComputationMethod.Value("COMPUTATION_METHOD_CORREL"),
            join_order, inp)

    def meshcode(self, join_order: Tuple[List[str], List[int], List[int]],
                 src: List) -> Dict:
        logger.info("meshcode request. "
                    f"[data_id list]={join_order[0]} "
                    f"[join method]={join_order[1]} "
                    f"[matching ID columns]={join_order[2]} "
                    f"[src columns]={src}")
        return self.__qmpc_server.execute_computation(
            ComputationMethod.Value("COMPUTATION_METHOD_MESH_CODE"),
            join_order, (src, []))

    def get_join_table(self,
                       join_order: Tuple[List[str], List[int], List[int]]) \
            -> Dict:
        logger.info("get_join_table request. "
                    f"[data_id list]={join_order[0]} "
                    f"[join method]={join_order[1]} "
                    f"[matching ID columns]={join_order[2]}")
        return self.__qmpc_server.execute_computation(
            ComputationMethod.Value("COMPUTATION_METHOD_JOIN_TABLE"),
            join_order, (join_order[2], []))

    def get_computation_result(self, job_uuid: str,
                               path: Optional[str] = None) -> Dict:
        logger.info("get_computation_result request. "
                    f"[job_uuid]={job_uuid} "
                    f"[path]={path}")
        return self.__qmpc_server.get_computation_result(job_uuid, path)

    def get_data_list(self) \
            -> Dict:
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
