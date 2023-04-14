from typing import Any, Dict

import pytest

from quickmpc.exception import ArgumentError, QMPCJobError, QMPCServerError
from quickmpc.qmpc_server import QMPCServer


def send_share_param(secrets=[[1, 2, 3]],
                     schema=["attr1", "attr2", "attr3"],
                     matching_column=1,
                     piece_size=1_000_000):
    return (secrets, schema, matching_column, piece_size)


def execute_computation_param(method_id=1,
                              data_ids=["data_id1", "data_id2"],
                              join=[1],
                              index=[1, 1],
                              src=[0],
                              target=[1]):
    return (method_id, (data_ids, join, index), (src, target))


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
            (send_share_param(secrets=[["a", 2, 3]]), ArgumentError),
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

    @pytest.mark.parametrize(
        ("params"), [
            execute_computation_param(method_id=1, target=[]),
            execute_computation_param(method_id=2, target=[]),
            execute_computation_param(method_id=3, target=[]),
            execute_computation_param(method_id=4),
            execute_computation_param(method_id=5),
            execute_computation_param(method_id=6),
        ]
    )
    def test_execute_computation(self, params,
                                 run_server1, run_server2, run_server3):
        """ serverに計算リクエストを送れるかのTest"""
        response: Dict[str, Any] = self.qmpc.execute_computation(*params)
        assert (response["is_ok"])

    @pytest.mark.parametrize(
        ("params", "expected_exception"), [
            # data_idsの要素数-1とjoinの要素数が一致していない
            (execute_computation_param(data_ids=["id1", "id2"], join=[1, 1]),
             ArgumentError),
            # data_idsの要素数とindexの要素数が一致していない
            (execute_computation_param(data_ids=["id1", "id2"], index=[1]),
             ArgumentError),
            # joinの値が範囲外
            (execute_computation_param(join=[-1]),
             ArgumentError),
            (execute_computation_param(join=[3]),
             ArgumentError),
            # QMPCJobErrorとして例外がthrowされるか
            (execute_computation_param(src=[1000000000]),
             QMPCJobError),
            # QMPCServerErrorとして例外がthrowされるか
            (execute_computation_param(data_ids=["UnregisteredDataId", "id2"]),
             QMPCServerError),
        ]
    )
    def test_execute_computation_errorhandring(self, params,
                                               expected_exception,
                                               run_server1,
                                               run_server2,
                                               run_server3):
        with pytest.raises(expected_exception):
            self.qmpc.execute_computation(*params)

    def test_get_computation_resultRequest(self, run_server1,
                                           run_server2, run_server3):
        """ serverから結果を得られるかのTest """
        job_uuid: str = "uuid"
        response: Dict[str, Any] = self.qmpc.get_computation_result(
            job_uuid, None)
        assert (response["is_ok"])

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

    qmpc_failed: QMPCServer = QMPCServer(
        # 通信失敗する場合をテストする用のサーバー
        ["http://localhost:50011",
         "http://localhost:50012",
         "http://localhost:50013"],
        "token_demo",
        10,
        0
    )

    @pytest.mark.parametrize(
        ("function", "argument"), [
            (qmpc_failed.send_share, send_share_param()),
            (qmpc_failed.delete_share, [[]]),
            (qmpc_failed.execute_computation, execute_computation_param()),
            (qmpc_failed.get_data_list, []),
            (qmpc_failed.get_elapsed_time, ["uuid"]),
            (qmpc_failed.get_computation_result, ["uuid", None]),
            (qmpc_failed.get_job_error_info, ["uuid"]),
        ]
    )
    def test_retry(self, function, argument, caplog,
                   run_server1, run_server2, run_server3):
        # 10回の retry に失敗したら "All 10 times it was an error" が log に出るかをテスト
        _ = function(*argument)
        assert "channel の準備が出来ません" in caplog.text
