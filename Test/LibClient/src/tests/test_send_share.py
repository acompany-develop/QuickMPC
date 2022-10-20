
import csv
import json

import pytest
from utils import qmpc


@pytest.mark.parametrize(
    ("table_file"),
    [
        ("table_data_5x5"),
        ("table_data_10x10"),
    ]
)
def test_send_share(table_file: str):

    # csv dataをパースする
    filename: str = f"Data/{table_file}.csv"
    secrets, schema = qmpc.parse_csv_file(filename)

    # データをシェア化し送信する
    res = qmpc.send_share(secrets, schema)
    assert (res["is_ok"])

    # 冪等性のために消しておく
    qmpc.delete_share([res["data_id"]])
