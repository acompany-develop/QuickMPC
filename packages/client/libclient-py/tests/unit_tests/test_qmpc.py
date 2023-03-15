import decimal
from typing import Any, Dict

import pytest

from quickmpc.exception import ArgumentError, QMPCJobError, QMPCServerError
from quickmpc.qmpc_server import QMPCServer


def send_share_param(secrets=[[1, 2, 3]],
                     schema=["attr1", "attr2", "attr3"],
                     matching_column=1,
                     piece_size=1_000_000):
    return (secrets, schema, matching_column, piece_size)


class TestQMPC:
    qmpc: QMPCServer = QMPCServer(
        ["http://localhost:50001",
         "http://localhost:50002",
         "http://localhost:50003"],
        "token_demo"
    )

    @pytest.mark.parametrize(
        ("params"), [
            (send_share_param()),
            (send_share_param(secrets=[[1], [2], [3]], schema=["a1"])),
            (send_share_param(secrets=[[0, 0, 0]])),
            (send_share_param(secrets=[[1e10, 1e10, 1e10]])),
            (send_share_param(secrets=[[-1e10, -1e10, -1e10]])),
            (send_share_param(secrets=[[1e-10, 1e-10, 1e-10]])),
            (send_share_param(secrets=[[-1e-10, -1e-10, -1e-10]])),
        ]
    )
    def test_send_shares(self, params,
                         run_server1, run_server2, run_server3):
        """ serverにシェアを送れるかのTest"""
        response: Dict[str, Any] = self.qmpc.send_share(*params)
        assert (response["is_ok"])

    @pytest.mark.parametrize(
        ("params", "expected_exception"), [
            # piece_sizeが範囲外
            (send_share_param(piece_size=500), RuntimeError),
            (send_share_param(piece_size=10000000), RuntimeError),
            # matching_columnが範囲外
            (send_share_param(matching_column=-1), RuntimeError),
            (send_share_param(matching_column=4), RuntimeError),
            # empty
            (send_share_param(secrets=[]), TypeError),
            # schemaに同じものが含まれる
            (send_share_param(schema=["a1", "a1", "a2"]), RuntimeError),
            # 列数が異なる
            (send_share_param(secrets=[[1, 1, 2], [2, 3]]), RuntimeError),
            # 文字列が含まれる
            (send_share_param(secrets=[["a", 2, 3]]),
             decimal.InvalidOperation),
            # Noneが含まれる
            (send_share_param(secrets=[[None, 2, 3]]), TypeError),
        ]
    )
    def test_send_shares_errorhandring(self, params, expected_exception,
                                       run_server1, run_server2, run_server3):
        with pytest.raises(expected_exception):
            self.qmpc.send_share(*params)

    def test_delete_shares(self, run_server1, run_server2, run_server3):
        """ serverにシェア削除要求を送れるかのTest"""
        response: Dict[str, Any] = self.qmpc.delete_share([])
        assert (response["is_ok"])

    def test_execute_computation(self, run_server1, run_server2, run_server3):
        """ serverに計算リクエストを送れるかのTest"""
        for method_id in range(1, 3):
            response: Dict[str, Any] = self.qmpc.execute_computation(
                method_id,
                [["id1", "id2"], [0], [1, 1]], [[0, 1], []])
            assert (response["is_ok"])

    def test_execute_computation_errorhandring(self, run_server1,
                                               run_server2, run_server3):
        with pytest.raises(ArgumentError):
            # data_idsの要素数-1とjoinの要素数が一致していない
            self.qmpc.execute_computation(
                1,
                [["id1", "id2"], [], [1, 1]], [[0, 1], []])
        with pytest.raises(ArgumentError):
            # data_idsの要素数とindexの要素数が一致していない
            self.qmpc.execute_computation(
                1,
                [["id1", "id2"], [0], [1]], [[0, 1], []])
        with pytest.raises(ArgumentError):
            # joinの値が0より小さい
            self.qmpc.execute_computation(
                1,
                [["id1", "id2"], [-1], [1, 1]], [[0, 1], []])
        with pytest.raises(ArgumentError):
            # joinの値が2より大きい
            self.qmpc.execute_computation(
                1,
                [["id1", "id2"], [3], [1, 1]], [[0, 1], []])

    def test_get_computation_resultRequest(self, run_server1,
                                           run_server2, run_server3):
        """ serverから結果を得られるかのTest """
        job_uuid: str = "uuid"
        response: Dict[str, Any] = self.qmpc.get_computation_result(
            job_uuid, None)
        assert (response["is_ok"])

    def test_exception_job_error(self):
        # QMPCJobErrorとして例外がthrowされるかのテスト
        with pytest.raises(QMPCJobError):
            self.qmpc.execute_computation(
                1,
                [["id1"], [], [1]], [[1000000000], []])

    def test_exception_server_error(self):
        # QMPCServerErrorとして例外がthrowされるかのテスト
        with pytest.raises(QMPCServerError):
            self.qmpc.execute_computation(
                1,
                [["UnregisteredDataId"], [], [1]], [[], []])

    def test_get_data_list(self, run_server1, run_server2, run_server3):
        """ serverにシェアを送れるかのTest"""
        response: Dict[str, Any] = self.qmpc.get_data_list()
        assert (response["is_ok"])

    def test_get_job_error_info(self, run_server1, run_server2, run_server3):
        # 例外がthrowされないことをテスト
        response: Dict[str, Any] = self.qmpc.get_job_error_info("test")

        assert (response["is_ok"])
        for res in response["job_error_info"]:
            assert (res.what == "QMPCJobError")
