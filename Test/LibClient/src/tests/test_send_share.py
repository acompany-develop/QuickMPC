
import csv
import json

import pytest
from utils import qmpc


@pytest.mark.parametrize(
    ("table_file"),
    [
        ("table_data_5x5"),
        ("table_data_10x10"),
        ("table_data_100x10"),
        ("table_data_1000x10"),
        ("table_data_10000x10"),
        ("table_data_100x100"),
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


@pytest.mark.parametrize(
    ("model_file"),
    [
        ("model_data_a6"),
        ("model_data_a11"),
    ]
)
def test_send_model_params_array(model_file: str):

    # csv dataを読み込む
    filename: str = f"Data/{model_file}.csv"
    data = []
    with open(filename) as f:
        reader = csv.reader(f, quoting=csv.QUOTE_NONNUMERIC)
        data = [row for row in reader][0]

    # データをシェア化し送信する
    res = qmpc.send_model_params(data)
    assert (res["is_ok"])


@pytest.mark.parametrize(
    ("model_file"),
    [
        ("model_data_j5_1"),
        ("model_data_j5_2"),
        ("model_data_j10_1"),
        ("model_data_j10_2"),
    ]
)
def test_send_model_params_json(model_file: str):

    # json dataを読み込む
    filename: str = f"Data/{model_file}.json"
    data = []
    with open(filename) as f:
        data = json.load(f)

    # データをシェア化し送信する
    res = qmpc.send_model_params(data)
    assert (res["is_ok"])
