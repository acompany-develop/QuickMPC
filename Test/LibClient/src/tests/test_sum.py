import math

import numpy as np
import pytest
from utils import get_result, qmpc

from .common import send_share


@pytest.mark.parametrize(
    ("table_file"),
    [
        # NOTE: テスト時間が長くなるためテストケースを一部省略
        ("table_data_5x5"),
        # ("table_data_10x10"),
        # ("table_data_100x10"),
        # ("table_data_1000x10"),
        # ("table_data_10000x10"),
        ("table_data_100x100"),
    ]
)
def test_sum(table_file: str):

    # share送信
    secrets, data_id = send_share(f"Data/{table_file}.csv")

    # table情報と列指定情報を定義して計算
    length = len(secrets[0])
    table = [[data_id], [], [1]]
    inp = [i for i in range(2, length+1)]

    res = get_result(qmpc.sum(table, inp))
    assert (res["is_ok"])

    # 正しく計算されたか確認
    secrets_np = np.array(secrets)[:, 1:]
    true_val = np.add.reduce(secrets_np)
    for x, y in zip(res["results"], true_val):
        assert (math.isclose(x, y, abs_tol=0.1))

    # 冪等性のために消しておく
    qmpc.delete_share([data_id])
