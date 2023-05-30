import csv
import json
import math
from dataclasses import dataclass, field
from typing import Callable, List

import pytest
from utils import get_result, qmpc

from tests.common import data_id


@dataclass
class Addtable:
    value: List[int]
    it: int = 0

    def f(self, row):
        row[0] += self.value[self.it]
        self.it += 1
        return row


def csv_data_param(secrets, row_process=lambda r: r):
    schema = [f"s{i}" for i in range(len(secrets[0]))]
    return ([schema]+secrets, row_process, 1, 1_000_000)


@pytest.mark.parametrize(
    ("secrets1", "add_table1", "secrets2", "expected"),
    [
        # 0 加算case
        ([["id1", 1, 3], ["id2", 2, 4]], Addtable([0, 0]),
         [["id1", 5], ["id2", 6]],
         [[1, 3, 5], [2, 4, 6]]),

        # 正 加算case
        ([["id1", 1, 3], ["id2", 2, 4]], Addtable([1, 2]),
         [["id1", 5], ["id2", 6]],
         [[1, 3, 5], [2, 4, 6]]),

        # 負 加算case
        ([["id1", 1, 3], ["id2", 2, 4]], Addtable([-1, -2]),
         [["id1", 5], ["id2", 6]],
         [[1, 3, 5], [2, 4, 6]]),
    ]
)
def test_add_value_join(secrets1: list, add_table1: Addtable,
                        secrets2: list, expected: list):

    # データをシェア化し送信する
    res = qmpc.send_share_from_csv_data(*csv_data_param(secrets1,
                                                        add_table1.f))
    assert (res["is_ok"])
    data_id1 = res["data_id"]
    res = qmpc.send_share_from_csv_data(*csv_data_param(secrets2))
    qmpc.add_value_to_id(data_id1, [str(-x) for x in add_table1.value], 1)
    assert (res["is_ok"])
    data_id2 = res["data_id"]

    # 計算リクエスト送信
    res = get_result(qmpc.get_join_table(([data_id1, data_id2], [2], [1, 1])))
    assert (res["is_ok"])

    # 正しく計算されたか確認
    assert len(res["results"]["table"]) == len(expected)
    for xl, yl in zip(res["results"]["table"], expected):
        assert len(xl) == len(yl)
        for x, y in zip(xl, yl):
            assert (math.isclose(x, y, abs_tol=0.1))

    # 冪等性のために消しておく
    qmpc.delete_share([data_id1, data_id2])
