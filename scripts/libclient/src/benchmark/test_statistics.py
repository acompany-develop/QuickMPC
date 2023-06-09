import math
import csv
import os

import numpy as np
import pytest
from utils import get_result, qmpc

from benchmark.metrics import PrintTime


@pytest.fixture(scope="module")
def get_data_id():
    val: dict = {}

    def get(path: str):
        if path in val:
            return val[path]
        with PrintTime("send_share"):
            res = qmpc.send_share_from_csv_file(path)
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
        (1, "data/data_10m.csv")
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
            res, job_uuid = get_result(qmpc.mean([data_id], inp), limit=10000)
        assert (res["is_ok"])


@pytest.mark.parametrize(
    ("iterate_num", "path"), test_parameters
)
def test_sum(iterate_num: int, path: int, get_data_id):
    data_id, _ = get_data_id(path)
    for _ in range(iterate_num):
        # table情報と列指定情報を定義して計算
        inp = [2]
        with PrintTime("sum"):
            res, job_uuid = get_result(qmpc.sum([data_id], inp), limit=10000)
        assert (res["is_ok"])


@pytest.mark.parametrize(
    ("iterate_num", "path"), test_parameters
)
def test_variance(iterate_num: int, path: int, get_data_id):
    data_id, _ = get_data_id(path)
    for _ in range(iterate_num):
        # table情報と列指定情報を定義して計算
        inp = [2]
        with PrintTime("variance"):
            res, job_uuid = get_result(qmpc.variance([data_id], inp), limit=10000)
        assert (res["is_ok"])


@pytest.mark.parametrize(
    ("iterate_num", "path"), test_parameters
)
def test_correl(iterate_num: int, path: int, get_data_id):
    data_id, schema_size = get_data_id(path)
    for _ in range(iterate_num):
        # table情報と列指定情報を定義して計算
        inp = ([2], [schema_size])
        with PrintTime("correl"):
            res, job_uuid = get_result(qmpc.correl([data_id], inp), limit=10000)
        assert (res["is_ok"])


@pytest.mark.parametrize(
    ("iterate_num", "path"), test_parameters
)
def test_hjoin(iterate_num: int, path: int, get_data_id):
    data_id, _ = get_data_id(path)
    for _ in range(iterate_num):
        # table情報を定義して計算
        with PrintTime("hjoin"):
            res, job_uuid = get_result(qmpc.get_join_table(
                [data_id, data_id]), limit=10000)
        assert (res["is_ok"])
