import math

import numpy as np
import pytest
from utils import get_result, qmpc

from .common import send_share


@pytest.mark.parametrize(
    ("table_file"),
    [
        # NOTE: 大きいケースは計算が遅いので省略
        ("table_data_5x5"),
        ("table_data_10x10"),
        # ("table_data_100x10"),
        # ("table_data_1000x10"),
        # ("table_data_10000x10"),
        # ("table_data_100x100"),
    ]
)
def test_linear_regression(table_file: str):
    # NOTE: 探索要素があるため，真値との比較はできない

    # share送信
    secrets, data_id = send_share(f"Data/{table_file}.csv")

    # table情報と列指定情報を定義して計算
    # 最後の列を目的値として計算
    length = len(secrets[0])
    table = [[data_id], [], [1]]
    inp = [[i for i in range(2, length)], [length]]

    res = get_result(qmpc.linear_regression(table, inp))
    assert (res["is_ok"])

    # 冪等性のために消しておく
    qmpc.delete_share([data_id])
