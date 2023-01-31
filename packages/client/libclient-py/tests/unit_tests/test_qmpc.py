from typing import Any, Dict

import pytest

from quickmpc.exception import ArgmentError, QMPCJobError, QMPCServerError
from quickmpc.qmpc_server import QMPCServer


class TestQMPC:
    qmpc: QMPCServer = QMPCServer(
        ["http://localhost:9001",
         "http://localhost:9002",
         "http://localhost:9003"],
        "token_demo"
    )

    @pytest.mark.parametrize(
        ("secrets", "schema"), [
            ([[1, 2, 3]], ["attr1", "attr2", "attr3"]),
            ([[1], [2], [3]], ["attr1"]), ]
    )
    def test_send_shares(self, secrets, schema,
                         run_server1, run_server2, run_server3):
        """ serverにシェアを送れるかのTest"""
        response: Dict[str, Any] = self.qmpc.send_share(
            secrets, schema, 1, 1000)
        assert (response["is_ok"])

    def test_send_shares_errorhandring(self,
                                       run_server1, run_server2, run_server3):
        with pytest.raises(RuntimeError):
            # piece_sizeが1000より小さい
            self.qmpc.send_share(
                [[1, 2, 3]], ["attr1", "attr2", "attr3"], 1, 500)
        with pytest.raises(RuntimeError):
            # piece_sizeが1000000より大きい
            self.qmpc.send_share(
                [[1, 2, 3]], ["attr1", "attr2", "attr3"], 1, 10000000)
        with pytest.raises(RuntimeError):
            # matching_columnが範囲外
            self.qmpc.send_share(
                [[1, 2, 3]], ["attr1", "attr2", "attr3"], -1, 1000)
        with pytest.raises(RuntimeError):
            # matching_columnが範囲外
            self.qmpc.send_share(
                [[1, 2, 3]], ["attr1", "attr2", "attr3"], 4, 1000)
        with pytest.raises(TypeError):
            # シェアがない
            self.qmpc.send_share(
                [], ["attr1", "attr2", "attr3"], 1, 1000)
        with pytest.raises(RuntimeError):
            # schemaに同じものが含まれる
            self.qmpc.send_share(
                [[1, 1, 2], [2, 3, 4]], ["attr1", "attr2", "attr2"], 1, 1000)
        with pytest.raises(RuntimeError):
            # 行によって列数が異なる
            self.qmpc.send_share(
                [[1, 1, 2], [2, 3, 4, 5]],
                ["attr1", "attr2", "attr3"], 1, 1000)

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
        with pytest.raises(ArgmentError):
            # data_idsの要素数-1とjoinの要素数が一致していない
            self.qmpc.execute_computation(
                1,
                [["id1", "id2"], [], [1, 1]], [[0, 1], []])
        with pytest.raises(ArgmentError):
            # data_idsの要素数とindexの要素数が一致していない
            self.qmpc.execute_computation(
                1,
                [["id1", "id2"], [0], [1]], [[0, 1], []])
        with pytest.raises(ArgmentError):
            # joinの値が0より小さい
            self.qmpc.execute_computation(
                1,
                [["id1", "id2"], [-1], [1, 1]], [[0, 1], []])
        with pytest.raises(ArgmentError):
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
