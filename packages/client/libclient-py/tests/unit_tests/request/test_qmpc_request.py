import io
from typing import List, Optional

import pandas as pd
import pytest

import quickmpc.pandas as qpd
from quickmpc.exception import QMPCServerError
from quickmpc.proto.common_types import common_types_pb2
from quickmpc.request.qmpc_request import QMPCRequest


def to_string_io(data: List[List]) -> io.StringIO:
    text_data = "\n".join([",".join(map(str, row)) for row in data])
    return io.StringIO(text_data)


def data_frame(values: List[List] = [[1, 2, 3], [3, 4, 5]],
               columns: Optional[List[str]] = None,
               exist_sort_index: bool = True
               ) -> pd.DataFrame:
    if columns is None:
        columns = [f"c{i}" for i in range(len(values[0]))] \
            if len(values) > 0 else []
    if columns and exist_sort_index:
        columns[-1] = "__qmpc_sort_index__"
    return pd.DataFrame(values, columns=columns)


def send_share_param(df: pd.DataFrame = data_frame(), piece_size: int = 1000):
    return (df, piece_size)


class TestQMPCRequest:
    qmpc_request = QMPCRequest([
        "http://localhost:50001",
        "http://localhost:50002",
        "http://localhost:50003"
    ])

    @pytest.mark.parametrize(
        ("params"), [
            # 通常ケース
            (send_share_param()),
            (send_share_param(df=data_frame([[0, 0, 0]]))),
            # 境界ケース
            (send_share_param(df=data_frame([[1e10, 1e10, 1e10]]))),
            (send_share_param(df=data_frame([[-1e10, -1e10, -1e10]]))),
            (send_share_param(df=data_frame([[1e-10, 1e-10, 1e-10]]))),
            (send_share_param(df=data_frame([[-1e-10, -1e-10, -1e-10]]))),
            (send_share_param(df=data_frame([[-1e-10, -1e-10, -1e-10]]))),
            # quickmpc.pandasのread_csv経由
            (send_share_param(df=qpd.read_csv(
                to_string_io([["id", "c"], ["a", 1]]), index_col="id"))),
        ]
    )
    def test_send_shares(self, params,
                         run_server1, run_server2, run_server3):
        self.qmpc_request.send_share(*params)

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
            (send_share_param(df=data_frame(columns=["a1", "a1", "a2"])),
             RuntimeError),
            # 列数が異なる
            (send_share_param(df=data_frame([[1, 1, 2], [2, 3]])),
             RuntimeError),
            # Noneが含まれる
            (send_share_param(df=data_frame([[None, 2, 3]])),
             RuntimeError),
            # __qmpc_sort_index__が含まれない
            (send_share_param(df=data_frame([[1, 2], [3, 4]],
                                            exist_sort_index=False)),
             RuntimeError),
            # __qmpc_sort_index__しかない
            (send_share_param(df=data_frame([[0], [1]])),
             RuntimeError),
            # pandasのread_csv経由
            (send_share_param(df=pd.read_csv(to_string_io([["id", "c"],
                                                           ["a", 1]]))),
             RuntimeError),
        ]
    )
    def test_send_shares_errorhandring(self, params, expected_exception,
                                       run_server1, run_server2, run_server3):
        with pytest.raises(expected_exception):
            self.qmpc_request.send_share(*params)

    def test_sum(self, run_server1, run_server2, run_server3):
        self.qmpc_request.sum(["data_id1"], [1, 2, 3])

    def test_mean(self, run_server1, run_server2, run_server3):
        self.qmpc_request.mean(["data_id1"], [1, 2, 3])

    def test_variance(self, run_server1, run_server2, run_server3):
        self.qmpc_request.variance(["data_id1"], [1, 2, 3])

    def test_correl(self, run_server1, run_server2, run_server3):
        self.qmpc_request.correl(["data_id1"], [1, 2, 3], [4])

    def test_meshcode(self, run_server1, run_server2, run_server3):
        self.qmpc_request.meshcode(["data_id1"], [1, 2, 3])

    def test_join(self, run_server1, run_server2, run_server3):
        self.qmpc_request.join(["data_id1"])

    def test_get_computation_result(self,
                                    run_server1, run_server2, run_server3):
        self.qmpc_request.get_computation_result("job_uuid", None)

    def test_get_computation_status(self,
                                    run_server1, run_server2, run_server3):
        response = self.qmpc_request.get_computation_status("test")
        assert response.job_statuses == [
            common_types_pb2.COMPLETED for _ in range(3)]

    def test_get_job_error_info(self, run_server1, run_server2, run_server3):
        response = self.qmpc_request.get_job_error_info("test")
        for res in response.job_error_info:
            assert res.what == "QMPCJobError"

    def test_get_elapsed_time(self, run_server1, run_server2, run_server3):
        self.qmpc_request.get_elapsed_time("job_uuid")

    def test_delete_share(self, run_server1, run_server2, run_server3):
        self.qmpc_request.delete_share(["data_id"])

    def test_add_share_data_frame(self, run_server1, run_server2, run_server3):
        self.qmpc_request.add_share_data_frame("id1", "id2")


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
            (qmpc_request_failed.get_elapsed_time, (["uuid"])),
            (qmpc_request_failed.get_computation_result, ["uuid", None]),
            (qmpc_request_failed.get_job_error_info, ["uuid"]),
            (qmpc_request_failed.add_share_data_frame, ["id1", "id2"]),
        ]
    )
    def test_retry(self, function, argument, caplog,
                   run_server1, run_server2, run_server3):
        with pytest.raises(QMPCServerError) as e:
            _ = function(*argument)
        assert str(e.value) == "channel の準備が出来ません"
