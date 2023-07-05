import csv
import math
import os
import time

import numpy as np
import pytest
from benchmark.metrics import PrintTime
from tabulate import tabulate
from utils import get_result, qmpc, qmpc_new

elapsed_time_dict: dict = {}


def save_elapsed_time(elapsed_time, job_name, path):
    if (path, job_name) in elapsed_time_dict:
        elapsed_time_dict[(path, job_name)] = max(
            elapsed_time_dict[(path, job_name)], elapsed_time)
    else:
        elapsed_time_dict[(path, job_name)] = elapsed_time


@pytest.fixture(scope="module")
def get_sdf():
    sdf_dict: dict = {}

    def get(path: str):
        if path in sdf_dict:
            return sdf_dict[path]
        df = qmpc_new.read_csv(path, index_col="id")
        with PrintTime("send_share") as pt:
            sdf = qmpc_new.send_to(df)
        save_elapsed_time(pt.elapsed_time, "send_share", path)
        schema_size = len(df.columns)
        sdf_dict[path] = (sdf, schema_size)
        return sdf_dict[path]
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
        (3, "data/table_data_10x10.csv"),
        (3, "data/table_data_100x10.csv"),
    ]


@pytest.mark.parametrize(
    ("iterate_num", "path"), test_parameters
)
def test_mean(iterate_num: int, path: int, get_sdf):
    sdf, _ = get_sdf(path)
    save_elapsed_time(sdf.mean([2]).get_elapsed_time(),
                      "mean", path)


@pytest.mark.parametrize(
    ("iterate_num", "path"), test_parameters
)
def test_sum(iterate_num: int, path: int, get_sdf):
    sdf, _ = get_sdf(path)
    save_elapsed_time(sdf.sum([2]).get_elapsed_time(),
                      "sum", path)


@pytest.mark.parametrize(
    ("iterate_num", "path"), test_parameters
)
def test_variance(iterate_num: int, path: int, get_sdf):
    sdf, _ = get_sdf(path)
    save_elapsed_time(sdf.variance([2]).get_elapsed_time(),
                      "variance", path)


@pytest.mark.parametrize(
    ("iterate_num", "path"), test_parameters
)
def test_correl(iterate_num: int, path: int, get_sdf):
    sdf, schema_size = get_sdf(path)
    save_elapsed_time(sdf.correl([2], [schema_size]).get_elapsed_time(),
                      "correl", path)


@pytest.mark.parametrize(
    ("iterate_num", "path"), test_parameters
)
def test_hjoin(iterate_num: int, path: int, get_sdf):
    sdf, schema_size = get_sdf(path)
    save_elapsed_time(sdf.join(sdf).get_elapsed_time(),
                      "hjoin", path)


def test_print_result():
    headers = ["path", "job", "elapsed_time[s]", "elapsed_time[m]"]
    data = []
    for key in elapsed_time_dict:
        elapsed_time_s = elapsed_time_dict[key]
        elapsed_time_m = elapsed_time_s / 60
        data.append([*key, elapsed_time_s, elapsed_time_m])
    data.sort()
    print("\n"+tabulate(data, headers=headers, tablefmt='fancy_grid'))
