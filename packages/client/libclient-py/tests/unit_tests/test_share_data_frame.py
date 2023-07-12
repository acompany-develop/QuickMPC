"""ShareDataFrameのテスト

mockサーバを使用しているので通信を要するmethodはロジックのテストまではされてない
"""
import io
from typing import List, Optional

import pandas as pd
import pytest

from quickmpc.pandas.share_data_frame import (ShareDataFrame,
                                              ShareDataFrameStatus)
from quickmpc.qmpc_request import QMPCRequest


def to_string_io(data: List[List]) -> io.StringIO:
    text_data = "\n".join([",".join(map(str, row)) for row in data])
    return io.StringIO(text_data)


def data_frame(values: List[List] = [[1, 2], [3, 4]],
               columns: Optional[List[str]] = None) -> pd.DataFrame:
    if columns is None:
        columns = [f"c{i}" for i in range(len(values[0]))] \
            if len(values) > 0 else []
    return pd.DataFrame(values, columns=columns)


ip_list = [
    "http://localhost:50001",
    "http://localhost:50002",
    "http://localhost:50003"
]


class TestShareDataFrame:
    qmpc_request = QMPCRequest(ip_list)

    def test_add(self,
                 run_server1, run_server2, run_server3):
        sdf1 = ShareDataFrame("data_id1", self.qmpc_request)
        sdf2 = ShareDataFrame("data_id2", self.qmpc_request)
        expected = ShareDataFrame("data_id", self.qmpc_request,
                                  False, ShareDataFrameStatus.OK)
        assert (sdf1+sdf2) == expected

    def test_join(self,
                  run_server1, run_server2, run_server3):
        sdf1 = ShareDataFrame("data_id1", self.qmpc_request)
        sdf2 = ShareDataFrame("data_id2", self.qmpc_request)
        expected = ShareDataFrame("job_uuid", self.qmpc_request,
                                  True, ShareDataFrameStatus.EXECUTE)
        assert sdf1.join(sdf2) == expected

    def test_sum(self,
                 run_server1, run_server2, run_server3):
        sdf = ShareDataFrame("data_id1", self.qmpc_request)
        expected = ShareDataFrame("job_uuid", self.qmpc_request,
                                  True, ShareDataFrameStatus.EXECUTE)
        assert sdf.sum([1, 2, 3]) == expected

    def test_mean(self,
                  run_server1, run_server2, run_server3):
        sdf = ShareDataFrame("data_id1", self.qmpc_request)
        expected = ShareDataFrame("job_uuid", self.qmpc_request,
                                  True, ShareDataFrameStatus.EXECUTE)
        assert sdf.mean([1, 2, 3]) == expected

    def test_variance(self,
                      run_server1, run_server2, run_server3):
        sdf = ShareDataFrame("data_id1", self.qmpc_request)
        expected = ShareDataFrame("job_uuid", self.qmpc_request,
                                  True, ShareDataFrameStatus.EXECUTE)
        assert sdf.variance([1, 2, 3]) == expected

    def test_correl(self,
                    run_server1, run_server2, run_server3):
        sdf = ShareDataFrame("data_id1", self.qmpc_request)
        expected = ShareDataFrame("job_uuid", self.qmpc_request,
                                  True, ShareDataFrameStatus.EXECUTE)
        assert sdf.correl([1, 2, 3], [4]) == expected

    def test_meshcode(self,
                      run_server1, run_server2, run_server3):
        sdf = ShareDataFrame("data_id1", self.qmpc_request)
        expected = ShareDataFrame("job_uuid", self.qmpc_request,
                                  True, ShareDataFrameStatus.EXECUTE)
        assert sdf.meshcode([1, 2, 3]) == expected

    def test_to_csv_is_result(self,
                              run_server1, run_server2, run_server3):
        sdf = ShareDataFrame("job_uuid", self.qmpc_request, True)
        output_path = "./tests/unit_tests/"
        sdf.to_csv(output_path)
        expected_after = ShareDataFrame("job_uuid", self.qmpc_request,
                                        True, ShareDataFrameStatus.OK)
        assert sdf == expected_after

    def test_to_csv_is_not_result(self,
                                  run_server1, run_server2, run_server3):
        sdf = ShareDataFrame("data_id", self.qmpc_request, False)
        output_path = "./tests/unit_tests/"
        with pytest.raises(RuntimeError):
            sdf.to_csv(output_path)

    def test_to_dataframe_is_result(self,
                                    run_server1, run_server2, run_server3):
        sdf = ShareDataFrame("job_uuid", self.qmpc_request, True)
        df = sdf.to_data_frame()
        expected = pd.DataFrame([3.0, 6.0])
        pd.testing.assert_frame_equal(df, expected)

    def test_to_dataframe_is_not_result(self,
                                        run_server1, run_server2, run_server3):
        sdf = ShareDataFrame("data_id", self.qmpc_request, False)
        with pytest.raises(RuntimeError):
            sdf.to_data_frame()
