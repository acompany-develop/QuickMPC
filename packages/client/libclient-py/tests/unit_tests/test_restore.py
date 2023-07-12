import pandas as pd

from quickmpc.pandas.share_data_frame import ShareDataFrame
from quickmpc.qmpc import QMPC
from quickmpc.request.qmpc_request import QMPCRequest


class TestQMPC:
    qmpc_request: QMPCRequest = QMPCRequest([
        "http://localhost:50001",
        "http://localhost:50002",
        "http://localhost:50003"
    ])
    qmpc: QMPC = QMPC(qmpc_request)

    def test_restore(self, run_server1, run_server2, run_server3):
        """ファイルに書き出し、復元できるかテスト"""
        job_uuid = "job_uuid"
        path = "./tests/unit_tests/"

        sdf = ShareDataFrame(job_uuid, self.qmpc_request, True)
        sdf.to_csv(path)

        df = self.qmpc.restore(job_uuid, path, 3)
        expected = pd.DataFrame([3.0, 6.0])
        pd.testing.assert_frame_equal(df, expected)
