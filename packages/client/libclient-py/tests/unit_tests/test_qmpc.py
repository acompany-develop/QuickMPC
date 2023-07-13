from typing import List, Optional

import pandas as pd
import pytest

from quickmpc.exception import ArgumentError
from quickmpc.pandas.share_data_frame import ShareDataFrameStatus
from quickmpc.qmpc import QMPC
from quickmpc.request.qmpc_request import QMPCRequest


def data_frame(values: List[List] = [[1, 2, 3], [3, 4, 5]],
               columns: Optional[List[str]] = None) -> pd.DataFrame:
    if columns is None:
        columns = [f"c{i}" for i in range(len(values[0]))] \
            if len(values) > 0 else []
    if columns:
        columns[-1] = "__qmpc_sort_index__"
    return pd.DataFrame(values, columns=columns)


ip_list = [
    "http://localhost:50001",
    "http://localhost:50002",
    "http://localhost:50003"
]


class TestQMPC:
    qmpc: QMPC = QMPC(ip_list)

    @pytest.mark.parametrize(
        ("arg"), [
            # IPをリストで渡すパターン
            (ip_list),
            # requestインスタンスを渡すパターン
            (QMPCRequest(ip_list)),
        ]
    )
    def test_constuctor(self, arg,
                        run_server1, run_server2, run_server3):
        QMPC(arg)

    @pytest.mark.parametrize(
        ("arg", "expected"), [
            # httpが記載されていない
            (["localhost:50001"], ArgumentError),
            # 文字列がIPでない
            (["HAHAHA!"], ArgumentError),
            # request interfaceにないオブジェクト
            (2, NotImplementedError),
            ("str", NotImplementedError),
            ({}, NotImplementedError),
        ]
    )
    def test_constuctor_raise(self, arg, expected,
                              run_server1, run_server2, run_server3):
        with pytest.raises(expected):
            QMPC(arg)

    def test_send_to(self,
                     run_server1, run_server2, run_server3):
        sdf = self.qmpc.send_to(data_frame())
        assert not sdf._ShareDataFrame__is_result
        assert sdf._ShareDataFrame__status == ShareDataFrameStatus.OK

    def test_load_from(self,
                       run_server1, run_server2, run_server3):
        data_id = "data_id"
        sdf = self.qmpc.load_from(data_id)
        assert sdf._ShareDataFrame__id == data_id
        assert not sdf._ShareDataFrame__is_result
        assert sdf._ShareDataFrame__status == ShareDataFrameStatus.OK
