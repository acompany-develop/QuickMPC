import csv
import json
import math
import random
import time

import numpy as np
import pytest
from utils import get_result, qmpc


@pytest.mark.parametrize(
    ("table_file,model_file"),
    [
        ("table_data_5x5", "model_data_a6"),
        ("table_data_10x10", "model_data_a11"),
    ]
)
def test_linear_regression_predict(table_file: str, model_file: str):

    # csv dataをパースして送信
    filename_table: str = f"Data/{table_file}.csv"
    secrets, schema = qmpc.parse_csv_file(filename_table)
    send_res = qmpc.send_share(secrets, schema)
    data_id: str = send_res["data_id"]

    # model dataを送信
    filename_model: str = f"Data/{model_file}.csv"
    data = []
    with open(filename_model) as f:
        reader = csv.reader(f, quoting=csv.QUOTE_NONNUMERIC)
        data = [row for row in reader][0]
    res_model = qmpc.send_model_params(data)
    model_uuid = res_model["job_uuid"]

    # table情報と列指定情報を定義して計算
    length = len(schema)
    table = [[data_id], [], [1]]
    inp = [i for i in range(2, length+1)]
    res = get_result(qmpc.linear_regression_predict(model_uuid, table, inp))
    assert (res["is_ok"])

    # 冪等性のために消しておく
    qmpc.delete_share([data_id])


@pytest.mark.parametrize(
    ("table_file,model_file"),
    [
        ("table_data_5x5", "model_data_a6"),
        ("table_data_10x10", "model_data_a11"),
    ]
)
def test_logistic_regression_predict(table_file: str, model_file: str):

    # csv dataをパースして送信
    filename_table: str = f"Data/{table_file}.csv"
    secrets, schema = qmpc.parse_csv_file(filename_table)
    send_res = qmpc.send_share(secrets, schema)
    data_id: str = send_res["data_id"]

    # model dataを送信
    filename_model: str = f"Data/{model_file}.csv"
    data = []
    with open(filename_model) as f:
        reader = csv.reader(f, quoting=csv.QUOTE_NONNUMERIC)
        data = [row for row in reader][0]
    res_model = qmpc.send_model_params(data)
    model_uuid = res_model["job_uuid"]

    # table情報と列指定情報を定義して計算
    length = len(schema)
    table = [[data_id], [], [1]]
    inp = [i for i in range(2, length+1)]
    res = get_result(qmpc.logistic_regression_predict(model_uuid, table, inp))
    assert (res["is_ok"])

    # 冪等性のために消しておく
    qmpc.delete_share([data_id])


@pytest.mark.parametrize(
    ("table_file,model_file"),
    [
        ("table_data_5x5", "model_data_j5_1"),
        ("table_data_5x5", "model_data_j5_2"),
        ("table_data_10x10", "model_data_j10_1"),
        ("table_data_10x10", "model_data_j10_2"),
    ]
)
def test_decision_tree_predict(table_file: str, model_file: str):

    # csv dataをパースして送信
    filename_table: str = f"Data/{table_file}.csv"
    secrets, schema = qmpc.parse_csv_file(filename_table)
    send_res = qmpc.send_share(secrets, schema)
    data_id: str = send_res["data_id"]

    # model dataを送信
    filename_model: str = f"Data/{model_file}.json"
    data = []
    with open(filename_model) as f:
        data = json.load(f)
    res_model = qmpc.send_model_params(data)
    model_uuid = res_model["job_uuid"]

    # table情報と列指定情報を定義して計算
    length = len(schema)
    table = [[data_id], [], [1]]
    inp = [i for i in range(2, length+1)]
    res = get_result(qmpc.decision_tree_predict(model_uuid, table, inp))
    assert (res["is_ok"])

    # 冪等性のために消しておく
    qmpc.delete_share([data_id])
