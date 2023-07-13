from typing import List, Optional

import pandas as pd
import pytest

from quickmpc.exception import ArgumentError
from quickmpc.pandas import ShareDataFrame
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
    request = QMPCRequest(ip_list)
    qmpc: QMPC = QMPC(request)

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

    @pytest.mark.parametrize(
        ("id_str", "expected"), [
            # data_id
            ("f8a6f7c7155476d54e98c45a633f152a"
             "b24368bc5f23292cef94a67df4f7aa7a",
             ShareDataFrame("f8a6f7c7155476d54e98c45a633f152a"
                            "b24368bc5f23292cef94a67df4f7aa7a",
                            request, False, ShareDataFrameStatus.OK)),
            # job_uuid
            ("b58a2037-d7d9-44f3-9858-ea8717a888dc",
             ShareDataFrame("b58a2037-d7d9-44f3-9858-ea8717a888dc",
                            request, True, ShareDataFrameStatus.EXECUTE)),
        ]
    )
    def test_load_from(self, id_str, expected,
                       run_server1, run_server2, run_server3):
        sdf = self.qmpc.load_from(id_str)
        assert sdf == expected

    @pytest.mark.parametrize(
        ("id_str"), [
            # hashだが桁が異なる
            ("f8a6f7c7155476d5"),
            ("f8a6f7c7155476d54e98c45a633f152a"),
            ("f8a6f7c7155476d54e98c45a633f152a"
             "b24368bc5f23292cef94a67df4f7aa7a"
             "f8a6f7c7155476d54e98c45a633f152a"
             "b24368bc5f23292cef94a67df4f7aa7a"),
            # uuidみたいだが桁が異なる
            ("b58a2037-d7d9-44f3-9858-ea8717a888d"),
            ("b58a2037-d7d9-44f3-985-ea8717a888dc"),
            ("b58a2037-d7d9-44f-9858-ea8717a888dc"),
            ("b58a2037-d7d-44f3-9858-ea8717a888dc"),
            ("b58a203-d7d9-44f3-9858-ea8717a888dc"),
            # 大文字が含まれる
            ("F8a6f7c7155476d54e98c45a633f152a"
             "b24368bc5f23292cef94a67df4f7aa7a"),
            ("B58a2037-d7d9-44f3-9858-ea8717a888dc"),
            # 記号が含まれる
            ("@8a6f7c7155476d54e98c45a633f152a"
             "b24368bc5f23292cef94a67df4f7aa7a"),
            ("=58a2037-d7d9-44f3-9858-ea8717a888dc"),
        ]
    )
    def test_load_from_raise(self, id_str,
                             run_server1, run_server2, run_server3):
        with pytest.raises(RuntimeError):
            self.qmpc.load_from(id_str)
