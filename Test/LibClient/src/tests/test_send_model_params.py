import csv
import json

import pytest
from utils import qmpc


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
