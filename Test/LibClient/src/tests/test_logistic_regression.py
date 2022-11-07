import math
import random

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
def test_logistic_regression(table_file: str):
    # NOTE: 探索要素があるため，真値との比較はできない

    # share送信
    secrets, schema = qmpc.parse_csv_file(f"Data/{table_file}.csv")
    send_res = qmpc.send_share(secrets, schema)
    data_id = send_res["data_id"]

    # 目的値として01の値が必要なので用意する
    secrets01 = [[s[0]] + [0 if random.random() < 0.5 else 1] for s in secrets]
    send_res01 = qmpc.send_share(secrets01, ["attr01"])
    data_id01: str = send_res01["data_id"]

    # table情報と列指定情報を定義して計算
    # 最後の列を目的値として計算
    length = len(secrets[0])
    table = [[data_id, data_id01], [0], [1, 1]]
    inp = [[i for i in range(2, length)], [length+1]]

    res = get_result(qmpc.logistic_regression(table, inp))
    assert (res["is_ok"])
