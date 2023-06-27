from typing import Any, Dict, List, Optional

import pandas as pd
import pytest

from quickmpc.qmpc_request import QMPCRequest
from quickmpc.request.status import Status

local_ip_list = [
    "http://localhost:50001",
    "http://localhost:50002",
    "http://localhost:50003"
]


def data_frame(values: List[List] = [[1, 2], [3, 4]],
               columns: Optional[List[str]] = None) -> pd.DataFrame:
    if columns is None:
        columns = [f"c{i}" for i in range(len(values[0]))] \
            if len(values) > 0 else []
    return pd.DataFrame(values, columns=columns)


def send_share_param(df: pd.DataFrame = data_frame(), piece_size: int = 1000):
    return (df, piece_size)


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
        response = self.qmpc_request.meshcode(["data_id1"], [1, 2, 3])
        assert response.status == Status.OK

    def test_join(self, run_server1, run_server2, run_server3):
        response = self.qmpc_request.join(["data_id1"])
        assert response.status == Status.OK

    def test_get_computation_result(self,
                                    run_server1, run_server2, run_server3):
        response = self.qmpc_request.get_computation_result("job_uuid", None)
        assert response.status == Status.OK

    def test_get_job_error_info(self, run_server1, run_server2, run_server3):
        response = self.qmpc_request.get_job_error_info("test")
        assert response.status == Status.OK
        for res in response.job_error_info:
            assert res.what == "QMPCJobError"

    def test_get_elapsed_time(self, run_server1, run_server2, run_server3):
        response = self.qmpc_request.get_elapsed_time("job_uuid")
        assert response.status == Status.OK

    def test_get_data_list(self, run_server1, run_server2, run_server3):
        response = self.qmpc_request.get_data_list()
        assert response.status == Status.OK

    def test_delete_share(self, run_server1, run_server2, run_server3):
        response = self.qmpc_request.delete_share(["data_id"])
        assert response.status == Status.OK


class TestQMPCRequestFailed:
    # 通信失敗する場合をテストする用のサーバー
    qmpc_request_failed = QMPCRequest(
        ["http://localhost:33333",
         "http://localhost:44444",
         "http://localhost:55555"],
        10,
        0
    )

    @pytest.mark.parametrize(
        ("function", "argument"), [
            (qmpc_request_failed.send_share, send_share_param()),
            (qmpc_request_failed.delete_share, ["data_id"]),
            (qmpc_request_failed.sum, (["data_id"], [1])),
            (qmpc_request_failed.get_data_list, ([])),
            (qmpc_request_failed.get_elapsed_time, (["uuid"])),
            (qmpc_request_failed.get_computation_result, ["uuid", None]),
            (qmpc_request_failed.get_job_error_info, ["uuid"]),
        ]
    )
    def test_retry(self, function, argument, caplog,
                   run_server1, run_server2, run_server3):
        print(argument, *argument)
        # 10回の retry に失敗したら "All 10 times it was an error" が log に出るかをテスト
        _ = function(*argument)
        assert "channel の準備が出来ません" in caplog.text
