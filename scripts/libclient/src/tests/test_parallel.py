import csv
import json
import math
from concurrent.futures import ThreadPoolExecutor

import numpy as np
import pytest
from utils import get_result, qmpc
from quickmpc import parse

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
    filename: str = "data/table_data_5x5.csv"

    # 並列にsend_share
    executor = ThreadPoolExecutor()
    futures = []
    for _ in range(parallel_num):
        futures.append(
            executor.submit(qmpc.send_share_from_csv_file, filename)
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
def test_execute(parallel_num: int):
    filename: str = "data/table_data_5x5.csv"
    res = qmpc.send_share_from_csv_file(filename)
    data_id = res["data_id"]

    # table情報と列指定情報を定義して計算
    secrets, schema = parse(filename)
    length: int = len(secrets[0])
    table = [[data_id], [], [1]]
    inp = [i for i in range(2, length+1)]

    res = get_result(qmpc.mean(table, inp))
    assert (res["is_ok"])

    # 並列にexecute_computation
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


@pytest.mark.skip(reason="send_model_param削除によりjob_uuidを取得できなくなったため")
@pytest.mark.parametrize(
    ("parallel_num"), parallel_num
)
def test_get_computation_result(parallel_num: int):
    # TODO: job_uuidを取得
    job_uuid = None

    # 並列にget_computation_result
    executor = ThreadPoolExecutor()
    futures = []
    for _ in range(parallel_num):
        futures.append(
            executor.submit(qmpc.get_computation_result, job_uuid)
        )

    for future in futures:
        res = future.result()
        assert (res["is_ok"])
