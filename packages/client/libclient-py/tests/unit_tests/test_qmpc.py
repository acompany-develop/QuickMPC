import io
from typing import List, Optional

import pandas as pd
import pytest

from quickmpc.exception import ArgumentError
from quickmpc.qmpc import QMPC
from quickmpc.qmpc_request import QMPCRequest
from quickmpc.share_data_frame import ShareDataFrameStatus


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

    @pytest.mark.parametrize(
        ("data", "index_col", "expected"), [
            # 通常パターン
            ([["id", "c"], ["a", 1], ["b", 2]],
             "id",
             pd.DataFrame([[32772040.0, 1], [86407020.0, 2]],
                          columns=["id", "c"],
                          index=[0, 1])),
            # 座圧されたID列がindexにあるかどうか
            ([["id", "c"], ["b", 2], ["a", 1]],
             "id",
             pd.DataFrame([[86407020.0, 2], [32772040.0, 1]],
                          columns=["id", "c"],
                          index=[1, 0])),
            # 1列目以外をID列に指定した場合
            ([["id", "c"], ["a", 1], ["b", 2]],
             "c",
             pd.DataFrame([["a", 1.0], ["b", 2.0]],
                          columns=["id", "c"],
                          index=[0, 1])),
        ]
    )
    def test_read_csv(self, data, index_col, expected,
                      run_server1, run_server2, run_server3):
        df = self.qmpc.read_csv(to_string_io(data), index_col=index_col)
        pd.testing.assert_frame_equal(df, expected)

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
