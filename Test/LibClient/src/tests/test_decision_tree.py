import numpy as np
import pytest
from utils import get_result, qmpc

from .common import send_share


@pytest.mark.parametrize(
    ("table_file"),
    [
        ("table_data_5x5"),
    ]
)
def test_decision_tree(table_file: str):
    # NOTE: 探索要素があるため，真値との比較はできない

    # share送信
    secrets, schema = qmpc.parse_csv_file_to_bitvector(
        f"Data/{table_file}.csv")
    send_res = qmpc.send_share(secrets, schema)
    data_id = send_res["data_id"]

    # table情報と列指定情報を定義して計算
    # 最後の列を目的値として計算(bitvector化しているため2列分)
    table = [[data_id], [], [1]]
    inp = ([2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13], [19, 20])

    res = get_result(qmpc.decision_tree(table, inp))
    assert (res["is_ok"])
