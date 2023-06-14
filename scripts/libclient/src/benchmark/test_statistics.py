import math
import csv
import time
import os
from tabulate import tabulate

import numpy as np
import pytest
from utils import get_result, qmpc

from benchmark.metrics import PrintTime


elapsed_time_dict: dict = {}
def save_elapsed_time(job_uuid, job_name, path):
    res = qmpc.get_elapsed_time(job_uuid)
    assert (res["is_ok"])
    if (path, job_name) in elapsed_time_dict:
        elapsed_time_dict[(path, job_name)] = max(elapsed_time_dict[(path, job_name)], res["elapsed_time"])
    else:
        elapsed_time_dict[(path, job_name)] = res["elapsed_time"]


@pytest.fixture(scope="module")
def get_data_id():
    val: dict = {}

    def get(path: str):
        if path in val:
            return val[path]
        with PrintTime("send_share") as pt:
            res = qmpc.send_share_from_csv_file(path)
        elapsed_time_dict[(path, "send_share")] = pt.elapsed_time
        assert (res["is_ok"])
        data_id: str = res["data_id"]
        with open(path) as f:
            reader = csv.reader(f)
            row = next(reader)
            schema_size: int = len(row)
        val[path] = (data_id, schema_size)
        return val[path]
    return lambda path: get(path)


# types: (iterate_num, path)
# testに使うファイルを指定
is_ci_benchmark = os.getenv("IS_CI_BENCHMARK")
if is_ci_benchmark:
    # CIでbenchmarkを実行する場合
    test_parameters = [
        #(1, "data/data_10m.csv")
        (1, "data/data_debug.csv")
    ]
else:
    # localで実行する場合にはここにデータを追加する
    test_parameters = [
        (10, "data/table_data_10x10.csv"),
        (10, "data/table_data_100x10.csv"),
    ]


@pytest.mark.parametrize(
    ("iterate_num", "path"), test_parameters
)
def test_mean(iterate_num: int, path: int, get_data_id):
    data_id, _ = get_data_id(path)
    for _ in range(iterate_num):
        # table情報と列指定情報を定義して計算
        inp = [2]
        with PrintTime("mean"):
            response = qmpc.mean([data_id], inp)
            res = get_result(response, limit=10000)
        assert (res["is_ok"])
        save_elapsed_time(response["job_uuid"], "mean", path)


@pytest.mark.parametrize(
    ("iterate_num", "path"), test_parameters
)
def test_sum(iterate_num: int, path: int, get_data_id):
    data_id, _ = get_data_id(path)
    for _ in range(iterate_num):
        # table情報と列指定情報を定義して計算
        inp = [2]
        with PrintTime("sum"):
            response = qmpc.sum([data_id], inp)
            res = get_result(response, limit=10000)
        assert (res["is_ok"])
        save_elapsed_time(response["job_uuid"], "sum", path)


@pytest.mark.parametrize(
    ("iterate_num", "path"), test_parameters
)
def test_variance(iterate_num: int, path: int, get_data_id):
    data_id, _ = get_data_id(path)
    for _ in range(iterate_num):
        # table情報と列指定情報を定義して計算
        inp = [2]
        with PrintTime("variance"):
            response = qmpc.variance([data_id], inp)
            res = get_result(response, limit=10000)
        assert (res["is_ok"])
        save_elapsed_time(response["job_uuid"], "variance", path)


@pytest.mark.parametrize(
    ("iterate_num", "path"), test_parameters
)
def test_correl(iterate_num: int, path: int, get_data_id):
    data_id, schema_size = get_data_id(path)
    for _ in range(iterate_num):
        # table情報と列指定情報を定義して計算
        inp = ([2], [schema_size])
        with PrintTime("correl"):
            response = qmpc.correl([data_id], inp)
            res = get_result(response, limit=10000)
        assert (res["is_ok"])
        save_elapsed_time(response["job_uuid"], "correl", path)


@pytest.mark.parametrize(
    ("iterate_num", "path"), test_parameters
)
def test_hjoin(iterate_num: int, path: int, get_data_id):
    data_id, _ = get_data_id(path)
    for _ in range(iterate_num):
        # table情報を定義して計算
        with PrintTime("hjoin"):
            response = qmpc.get_join_table([data_id, data_id])
            res = get_result(response, limit=10000)
        assert (res["is_ok"])
        save_elapsed_time(response["job_uuid"], "hjoin", path)


def test_print_result():
    headers = [ "path", "job", "elapsed_time[s]", "elapsed_time[m]"]
    data = []
    for key in elapsed_time_dict:
        elapsed_time_s = elapsed_time_dict[key]
        elapsed_time_m = elapsed_time_s / 60
        data.append([*key, elapsed_time_s, elapsed_time_m])
    data.sort()
    print("\n"+tabulate(data, headers=headers, tablefmt='fancy_grid'))
