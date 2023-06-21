from typing import Any, Dict, List, Optional, Union

import pandas as pd
import pytest

from quickmpc.exception import ArgumentError, QMPCJobError, QMPCServerError
from quickmpc.proto.common_types.common_types_pb2 import ComputationMethod
from quickmpc.qmpc_request import QMPCRequest
from quickmpc.request.response import SendShareResponse
from quickmpc.request.status import Status

local_ip_list = [
    "http://localhost:50001",
    "http://localhost:50002",
    "http://localhost:50003"
]


def data_frame(values: List[List] = [[1, 2], [3, 4]],
               index: Optional[List[float]] = None,
               columns: Optional[List[str]] = None) -> pd.DataFrame:
    if index is None:
        index = [i for i in range(len(values))]
    if columns is None:
        columns = [f"c{i}" for i in range(len(values[0]))] \
            if len(values) > 0 else []
    return pd.DataFrame(values, index=index, columns=columns)


def send_share_param(df: pd.DataFrame = data_frame(), piece_size: int = 1000):
    return (df, piece_size)


def execute_computation_param(method_id=1,
                              data_ids=["data_id1", "data_id2"],
                              join=[1],
                              src=[0],
                              target=[1]):
    return (method_id, (data_ids, join), (src, target))


class TestQMPCRequest:
    qmpc_request = QMPCRequest(local_ip_list)

    @pytest.mark.parametrize(
        ("params"), [
            (send_share_param()),
            (send_share_param(df=data_frame([[1], [2], [3]]))),
            (send_share_param(df=data_frame([[0, 0, 0]]))),
            (send_share_param(df=data_frame([[1e10, 1e10, 1e10]]))),
            (send_share_param(df=data_frame([[-1e10, -1e10, -1e10]]))),
            (send_share_param(df=data_frame([[1e-10, 1e-10, 1e-10]]))),
            (send_share_param(df=data_frame([[-1e-10, -1e-10, -1e-10]]))),
        ]
    )
    def test_send_shares(self, params,
                         run_server1, run_server2, run_server3):
        response = self.qmpc_request.send_share(*params)
        assert response.status == Status.OK

    @pytest.mark.parametrize(
        ("params", "expected_exception"), [
            # piece_sizeが範囲外
            (send_share_param(piece_size=500),
             RuntimeError),
            (send_share_param(piece_size=10000000),
             RuntimeError),
            # empty
            (send_share_param(df=data_frame([])),
             RuntimeError),
            # schemaに同じものが含まれる
            (send_share_param(df=data_frame(columns=["a1", "a1"])),
             RuntimeError),
            # 列数が異なる
            (send_share_param(df=data_frame([[1, 1, 2], [2, 3]])),
             RuntimeError),
            # Noneが含まれる
            (send_share_param(df=data_frame([[None, 2, 3]])),
             RuntimeError),
        ]
    )
    def test_send_shares_errorhandring(self, params, expected_exception,
                                       run_server1, run_server2, run_server3):
        with pytest.raises(expected_exception):
            self.qmpc_request.send_share(*params)

    def test_sum(self, run_server1, run_server2, run_server3):
        response = self.qmpc_request.sum(["data_id1"], [1, 2, 3])
        assert response.status == Status.OK

    def test_mean(self, run_server1, run_server2, run_server3):
        response = self.qmpc_request.mean(["data_id1"], [1, 2, 3])
        assert response.status == Status.OK

    def test_variance(self, run_server1, run_server2, run_server3):
        response = self.qmpc_request.variance(["data_id1"], [1, 2, 3])
        assert response.status == Status.OK

    def test_correl(self, run_server1, run_server2, run_server3):
        response = self.qmpc_request.correl(["data_id1"], [1, 2, 3], [4])
        assert response.status == Status.OK

    def test_meshcode(self, run_server1, run_server2, run_server3):
        response = self.qmpc_request.meshcode(["data_id1"], [1, 2, 3], [4])
        assert response.status == Status.OK

    def test_join(self, run_server1, run_server2, run_server3):
        response = self.qmpc_request.join(["data_id1"])
        assert response.status == Status.OK

    def test_get_computation_result(self,
                                    run_server1, run_server2, run_server3):
        response = self.qmpc_request.get_computation_result("job_uuid", None)
        assert response.status == Status.OK
