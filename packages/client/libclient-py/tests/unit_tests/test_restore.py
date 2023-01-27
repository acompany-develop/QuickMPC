import numpy as np

from quickmpc import QMPC


class TestQMPC:
    qmpc: QMPC = QMPC(
        ["http://localhost:9001",
         "http://localhost:9002",
         "http://localhost:9003"],
        "token_demo"
    )

    def test_restore(self, run_server1, run_server2, run_server3):
        """ファイルに書き出し、復元できるかテスト"""
        job_id: str = "uuid"
        path = "./tests/unit_tests/"
        res = self.qmpc.get_computation_result(job_id, path)
        assert (res["is_ok"])

        res = self.qmpc.restore(job_id, path)

        expect = [3.0, 6.0]
        assert (np.allclose(expect, res))
