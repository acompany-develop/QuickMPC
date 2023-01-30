import logging
from dataclasses import dataclass, field, InitVar
from typing import Dict, List, Optional, Tuple

from google.protobuf.internal import enum_type_wrapper

from .proto.common_types import common_types_pb2
from .qmpc_server import QMPCServer
from .share import Share
from .utils.parse_csv import (parse, parse_csv, parse_csv_to_bitvector,
                              parse_to_bitvector)
from .utils.restore import restore

logger = logging.getLogger(__name__)
# qmpc.JobStatus でアクセスできるようにエイリアスを設定する
JobStatus: enum_type_wrapper.EnumTypeWrapper \
    = common_types_pb2.JobStatus
ComputationMethod: enum_type_wrapper.EnumTypeWrapper \
    = common_types_pb2.ComputationMethod
JobErrorInfo = common_types_pb2.JobErrorInfo


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

    def parse_csv_file(self, filename: str) \
            -> Tuple[List[List[float]], List[str]]:
        logger.info("parse_csv_file. "
                    f"[filename]='{filename}'")
        return parse_csv(filename)

    def parse_csv_file_to_bitvector(self, filename: str,
                                    exclude: List[int] = []) \
            -> Tuple[List[List[float]], List[str]]:
        logger.info("parse_csv_file_to_bitvector. "
                    f"[filename]='{filename}' "
                    f"[not bitvector columns]={exclude}")
        return parse_csv_to_bitvector(filename, exclude)

    def parse_csv_data(self, data: List[List[str]]) \
            -> Tuple[List[List[float]], List[str]]:
        logger.info("parse_csv_data. "
                    f"[data size]={len(data)}x{len(data[0])}")
        return parse(data)

    def parse_csv_data_to_bitvector(self, data: List[List[str]],
                                    exclude: List[int] = []) \
            -> Tuple[List[List[float]], List[str]]:
        logger.info("parse_csv_file_to_bitvector. "
                    f"[data size]={len(data)}x{len(data[0])} "
                    f"[not bitvector columns]={exclude}")
        return parse_to_bitvector(data, exclude)

    def send_share(self, secrets: List, schema: List[str],
                   matching_column: int = 1,
                   piece_size: int = 1_000_000) -> Dict:
        logger.info("send_share request. "
                    f"[secrets size]={len(secrets)}x{len(secrets[0])} "
                    f"[matching ID name]={schema[matching_column-1]}")
        return self.__qmpc_server.send_share(
            secrets, schema, matching_column, piece_size)

    def delete_share(self, data_ids: List[str]) -> Dict:
        logger.info("delete_share request. "
                    f"[delete id list]={data_ids}")
        return self.__qmpc_server.delete_share(data_ids)

    def mean(self, join_order: Tuple[List, List, List],
             src: List) -> Dict:
        logger.info("mean request. "
                    f"[data_id list]={join_order[0]} "
                    f"[join method]={join_order[1]} "
                    f"[matching ID columns]={join_order[2]} "
                    f"[src columns]={src}")
        return self.__qmpc_server.execute_computation(
            ComputationMethod.Value("COMPUTATION_METHOD_MEAN"),
            join_order, (src, []))

    def variance(self, join_order: Tuple[List, List, List],
                 src: List) -> Dict:
        logger.info("variance request. "
                    f"[data_id list]={join_order[0]} "
                    f"[join method]={join_order[1]} "
                    f"[matching ID columns]={join_order[2]} "
                    f"[src columns]={src}")
        return self.__qmpc_server.execute_computation(
            ComputationMethod.Value("COMPUTATION_METHOD_VARIANCE"),
            join_order, (src, []))

    def sum(self, join_order: Tuple[List, List, List], src: List) -> Dict:
        logger.info("sum request. "
                    f"[data_id list]={join_order[0]} "
                    f"[join method]={join_order[1]} "
                    f"[matching ID columns]={join_order[2]} "
                    f"[src columns]={src}")
        return self.__qmpc_server.execute_computation(
            ComputationMethod.Value("COMPUTATION_METHOD_SUM"),
            join_order, (src, []))

    def correl(self, join_order: Tuple[List, List, List],
               inp: Tuple[List, List]) -> Dict:
        logger.info("correl request. "
                    f"[data_id list]={join_order[0]} "
                    f"[join method]={join_order[1]} "
                    f"[matching ID columns]={join_order[2]} "
                    f"[src columns]={inp[0]}"
                    f"[target columns]={inp[1]}")
        return self.__qmpc_server.execute_computation(
            ComputationMethod.Value("COMPUTATION_METHOD_CORREL"),
            join_order, inp)

    def linear_regression(self, join_order: Tuple[List, List, List],
                          inp: Tuple[List, List]) -> Dict:
        logger.info("linear_regression request. "
                    f"[data_id list]={join_order[0]} "
                    f"[join method]={join_order[1]} "
                    f"[matching ID columns]={join_order[2]} "
                    f"[design variable columns]={inp[0]}"
                    f"[objective variable columns]={inp[1]}")
        return self.__qmpc_server.execute_computation(
            ComputationMethod.Value("COMPUTATION_METHOD_LINEAR_REGRESSION"),
            join_order, inp)

    def logistic_regression(self, join_order: Tuple[List, List, List],
                            inp: Tuple[List, List]) -> Dict:
        logger.info("logistic_regression request. "
                    f"[data_id list]={join_order[0]} "
                    f"[join method]={join_order[1]} "
                    f"[matching ID columns]={join_order[2]} "
                    f"[design variable columns]={inp[0]}"
                    f"[objective variable columns]={inp[1]}")
        return self.__qmpc_server.execute_computation(
            ComputationMethod.Value("COMPUTATION_METHOD_LOGISTIC_REGRESSION"),
            join_order, inp)

    def meshcode(self, join_order: Tuple[List, List, List],
                 src: List) -> Dict:
        logger.info("meshcode request. "
                    f"[data_id list]={join_order[0]} "
                    f"[join method]={join_order[1]} "
                    f"[matching ID columns]={join_order[2]} "
                    f"[src columns]={src}")
        return self.__qmpc_server.execute_computation(
            ComputationMethod.Value("COMPUTATION_METHOD_MESH_CODE"),
            join_order, (src, []))

    def decision_tree(self, join_order: Tuple[List, List, List],
                      inp: Tuple[List, List]) -> Dict:
        logger.info("decision_tree request. "
                    f"[data_id list]={join_order[0]} "
                    f"[join method]={join_order[1]} "
                    f"[matching ID columns]={join_order[2]} "
                    f"[design variable columns]={inp[0]}"
                    f"[objective variable columns]={inp[1]}")
        return self.__qmpc_server.execute_computation(
            ComputationMethod.Value("COMPUTATION_METHOD_DECISION_TREE"),
            join_order, inp)

    def get_join_table(self, join_order: Tuple[List, List, List]) -> Dict:
        logger.info("get_join_table request. "
                    f"[data_id list]={join_order[0]} "
                    f"[join method]={join_order[1]} "
                    f"[matching ID columns]={join_order[2]}")
        return self.__qmpc_server.execute_computation(
            ComputationMethod.Value("COMPUTATION_METHOD_JOIN_TABLE"),
            join_order, (join_order[2], []))

    def get_computation_result(self, job_id: str,
                               path: Optional[str] = None) -> Dict:
        logger.info("get_computation_result request. "
                    f"[job_id]={job_id} "
                    f"[path]={path}")
        return self.__qmpc_server.get_computation_result(job_id, path)

    def get_data_list(self) \
            -> Dict:
        logger.info("get_data_list request.")
        return self.__qmpc_server.get_data_list()

    def demo_sharize(self, secrets: List) -> Dict:
        logger.info("demo_sharize request. "
                    f"[secrets size]={len(secrets)}x{len(secrets[0])}")
        share = Share.sharize(secrets, self.__party_size)
        return {'is_ok': True, 'results': share}

    def get_elapsed_time(self, job_id: str) -> Dict:
        logger.info("get_elapsed_time request. "
                    f"[job_id]={job_id}")
        return self.__qmpc_server.get_elapsed_time(job_id)

    def restore(self, job_id: str, path: str):
        logger.info("restore request. "
                    f"[job_id]={job_id} "
                    f"[path]={path}")
        return restore(job_id, path, self.__party_size)

    def get_job_error_info(self, job_id: str) -> Dict:
        logger.info("get_job_error_info request. "
                    f"[job_id]={job_id}")
        return self.__qmpc_server.get_job_error_info(job_id)

    @staticmethod
    def set_log_level(level: int):
        logger.setLevel(level)
