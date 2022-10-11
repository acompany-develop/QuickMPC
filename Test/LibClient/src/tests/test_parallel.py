import csv
import json
import math
from concurrent.futures import ThreadPoolExecutor

import numpy as np
import pytest
from utils import get_result, qmpc

parallel_num = [
    (5),
    (10),
    (50)
]


@pytest.mark.parametrize(
    ("parallel_num"), parallel_num
)
def test_send_share(parallel_num: int):
    # csv dataをパースする
    filename: str = "Data/table_data_5x5.csv"
    secrets, schema = qmpc.parse_csv_file(filename)

    executor = ThreadPoolExecutor()
    futures = []
    for _ in range(parallel_num):
        futures.append(
            executor.submit(qmpc.send_share, secrets, schema)
        )

    data_ids = []
    for future in futures:
        res = future.result()
        data_ids.append(res["data_id"])
        assert (res["is_ok"])

    # 冪等性のために消しておく
    qmpc.delete_share(data_ids)


@pytest.mark.parametrize(
    ("parallel_num"), parallel_num
)
def test_send_model_params_array(parallel_num: int):
    filename: str = "Data/model_data_a6.csv"
    data = []
    with open(filename) as f:
        reader = csv.reader(f, quoting=csv.QUOTE_NONNUMERIC)
        data = [row for row in reader][0]

    executor = ThreadPoolExecutor()
    futures = []
    for _ in range(parallel_num):
        futures.append(
            executor.submit(qmpc.send_model_params, data)
        )

    for future in futures:
        res = future.result()
        assert (res["is_ok"])


@pytest.mark.parametrize(
    ("parallel_num"), parallel_num
)
def test_send_model_params_json(parallel_num: int):
    filename: str = "Data/model_data_j5_1.json"
    data = []
    with open(filename) as f:
        data = json.load(f)

    executor = ThreadPoolExecutor()
    futures = []
    for _ in range(parallel_num):
        futures.append(
            executor.submit(qmpc.send_model_params, data)
        )

    for future in futures:
        res = future.result()
        assert (res["is_ok"])


@pytest.mark.parametrize(
    ("parallel_num"), parallel_num
)
def test_execute(parallel_num: int):
    filename: str = "Data/table_data_5x5.csv"
    secrets, schema = qmpc.parse_csv_file(filename)
    res = qmpc.send_share(secrets, schema)
    data_id = res["data_id"]

    # table情報と列指定情報を定義して計算
    length: int = len(secrets[0])
    table = [[data_id], [], [1]]
    inp = [i for i in range(2, length+1)]

    res = get_result(qmpc.mean(table, inp))
    assert (res["is_ok"])

    executor = ThreadPoolExecutor()
    futures = []
    for _ in range(parallel_num):
        futures.append(
            executor.submit(qmpc.sum, table, inp)
        )

    for future in futures:
        res = get_result(future.result())
        assert (res["is_ok"])

        # 正しく計算されたか確認
        secrets_np = np.array(secrets)[:, 1:]
        true_val = np.add.reduce(secrets_np)
        for x, y in zip(res["results"], true_val):
            assert (math.isclose(x, y, abs_tol=0.1))


@pytest.mark.skip(reason="Predict does not yet support parallelism.")
@pytest.mark.parametrize(
    ("parallel_num"), parallel_num
)
def test_predict(parallel_num: int):
    # csv dataをパースして送信
    filename_table: str = "Data/table_data_5x5.csv"
    secrets, schema = qmpc.parse_csv_file(filename_table)
    send_res = qmpc.send_share(secrets, schema)
    data_id: str = send_res["data_id"]

    # model dataを送信
    filename_model: str = "Data/model_data_a6.csv"
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

    executor = ThreadPoolExecutor()
    futures = []
    for _ in range(parallel_num):
        futures.append(
            executor.submit(qmpc.linear_regression_predict,
                            model_uuid, table, inp)
        )

    for future in futures:
        res = get_result(future.result())
        assert (res["is_ok"])

    # 冪等性のために消しておく
    qmpc.delete_share([data_id])


@pytest.mark.parametrize(
    ("parallel_num"), parallel_num
)
def test_get_computation_result(parallel_num: int):
    filename: str = "Data/model_data_j5_1.json"
    data = []
    with open(filename) as f:
        data = json.load(f)
    res = qmpc.send_model_params(data)
    job_uuid = res["job_uuid"]

    executor = ThreadPoolExecutor()
    futures = []
    for _ in range(parallel_num):
        futures.append(
            executor.submit(qmpc.get_computation_result, job_uuid)
        )

    for future in futures:
        res = future.result()
        assert (res["is_ok"])
