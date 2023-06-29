import numpy as np
import pandas as pd

from quickmpc import QMPC
from quickmpc.qmpc_new import QMPC as QMPCNew
from quickmpc.qmpc_request import QMPCRequest
from quickmpc.share_data_frame import ShareDataFrame


class TestQMPC:
    qmpc: QMPC = QMPC(
        ["http://localhost:50001",
         "http://localhost:50002",
         "http://localhost:50003"],
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


class TestQMPCNew:
    qmpc_request: QMPCRequest = QMPCRequest([
        "http://localhost:50001",
        "http://localhost:50002",
        "http://localhost:50003"
    ])
    qmpc: QMPCNew = QMPCNew(qmpc_request)

    def test_restore(self, run_server1, run_server2, run_server3):
        """ファイルに書き出し、復元できるかテスト"""
        job_uuid = "job_uuid"
        path = "./tests/unit_tests/"

        sdf = ShareDataFrame(job_uuid, self.qmpc_request, True)
        sdf.to_csv(path)

        df = self.qmpc.restore(job_uuid, path, 3)
        expected = pd.DataFrame([3.0, 6.0])
        pd.testing.assert_frame_equal(df, expected)
