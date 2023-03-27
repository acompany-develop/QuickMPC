import math

import numpy as np
import pytest
from utils import get_result, qmpc

from benchmark.data import large_data
from benchmark.metrics import PrintTime


@pytest.fixture(scope="module")
def get_data_id():
    val: dict = {}

    def get(size: int, data_num):
        if (size, data_num) in val:
            return val[(size, data_num)]
        secrets, schema = large_data(size, data_num)
        with PrintTime("send_share"):
            res = qmpc.send_share(secrets, schema)
        assert (res["is_ok"])
        data_id: str = res["data_id"]
        schema_size: int = len(schema)
        val[(size, data_num)] = (data_id, schema_size)
        return val[(size, data_num)]
    return lambda size, data_num=1: get(size, data_num)


# types: (iterate_num, size)
test_parameters = [
    (10, 5),
    (10, 50)
]


@pytest.mark.parametrize(
    ("iterate_num", "size"), test_parameters
)
def test_mean(iterate_num: int, size: int, get_data_id):
    data_id, schema_size = get_data_id(size)
    for _ in range(iterate_num):
        # table情報と列指定情報を定義して計算
        table = [[data_id], [], [1]]
        inp = [i+1 for i in range(schema_size)]
        with PrintTime("mean"):
            res = get_result(qmpc.mean(table, inp), limit=10000)
        assert (res["is_ok"])


@pytest.mark.parametrize(
    ("iterate_num", "size"), test_parameters
)
def test_sum(iterate_num: int, size: int, get_data_id):
    data_id, schema_size = get_data_id(size)
    for _ in range(iterate_num):
        # table情報と列指定情報を定義して計算
        table = [[data_id], [], [1]]
        inp = [i+1 for i in range(schema_size)]
        with PrintTime("sum"):
            res = get_result(qmpc.sum(table, inp), limit=10000)
        assert (res["is_ok"])


@pytest.mark.parametrize(
    ("iterate_num", "size"), test_parameters
)
def test_variance(iterate_num: int, size: int, get_data_id):
    data_id, schema_size = get_data_id(size)
    for _ in range(iterate_num):
        # table情報と列指定情報を定義して計算
        table = [[data_id], [], [1]]
        inp = [i+1 for i in range(schema_size)]
        with PrintTime("variance"):
            res = get_result(qmpc.variance(table, inp), limit=10000)
        assert (res["is_ok"])


@pytest.mark.parametrize(
    ("iterate_num", "size"), test_parameters
)
def test_correl(iterate_num: int, size: int, get_data_id):
    data_id, schema_size = get_data_id(size)
    for _ in range(iterate_num):
        # table情報と列指定情報を定義して計算
        table = [[data_id], [], [1]]
        inp = ([i+1 for i in range(schema_size)], [schema_size])
        with PrintTime("correl"):
            res = get_result(qmpc.correl(table, inp), limit=10000)
        assert (res["is_ok"])


@pytest.mark.parametrize(
    ("iterate_num", "size"), test_parameters
)
def test_hjoin(iterate_num: int, size: int, get_data_id):
    data_id1, _ = get_data_id(size, data_num=1)
    data_id2, _ = get_data_id(size, data_num=2)
    for _ in range(iterate_num):
        # table情報を定義して計算
        table = [[data_id1, data_id2], [2], [1, 1]]
        with PrintTime("hjoin"):
            res = get_result(qmpc.get_join_table(table), limit=10000)
        assert (res["is_ok"])
