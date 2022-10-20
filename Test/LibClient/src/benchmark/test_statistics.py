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
        val[(size, data_num)] = (data_id, secrets, schema)
        return val[(size, data_num)]
    return lambda size, data_num=1: get(size, data_num)


bench_size = [
    (5),
    (50),
    (500),
]


@pytest.mark.parametrize(
    ("size"), bench_size
)
def test_mean(size: int, get_data_id):
    data_id, secrets, schema = get_data_id(size)
    schema_size: int = len(schema)

    # table情報と列指定情報を定義して計算
    table = [[data_id], [], [1]]
    inp = [i+1 for i in range(schema_size)]
    with PrintTime("mean"):
        res = get_result(qmpc.mean(table, inp), limit=10000)
    assert (res["is_ok"])

    # 正しく計算されたか確認
    secrets_np = np.array(secrets)
    true_val = np.add.reduce(secrets_np) / len(secrets)
    for x, y in zip(res["results"], true_val):
        assert (math.isclose(x, y, abs_tol=0.1))


@pytest.mark.parametrize(
    ("size"), bench_size
)
def test_sum(size: int, get_data_id):
    data_id, secrets, schema = get_data_id(size)
    schema_size: int = len(schema)

    # table情報と列指定情報を定義して計算
    table = [[data_id], [], [1]]
    inp = [i+1 for i in range(schema_size)]
    with PrintTime("sum"):
        res = get_result(qmpc.sum(table, inp), limit=10000)
    assert (res["is_ok"])

    # 正しく計算されたか確認
    secrets_np = np.array(secrets)
    true_val = np.add.reduce(secrets_np)
    for x, y in zip(res["results"], true_val):
        assert (math.isclose(x, y, abs_tol=0.1))


@pytest.mark.parametrize(
    ("size"), bench_size
)
def test_variance(size: int, get_data_id):
    data_id, secrets, schema = get_data_id(size)
    schema_size: int = len(schema)

    # table情報と列指定情報を定義して計算
    table = [[data_id], [], [1]]
    inp = [i+1 for i in range(schema_size)]
    with PrintTime("variance"):
        res = get_result(qmpc.variance(table, inp), limit=10000)
    assert (res["is_ok"])

    # 正しく計算されたか確認
    secrets_np = np.array(secrets)
    true_val = np.var(secrets_np, axis=0)
    for x, y in zip(res["results"], true_val):
        assert (math.isclose(x, y, abs_tol=0.1))


@pytest.mark.parametrize(
    ("size"), bench_size
)
def test_correl(size: int, get_data_id):
    data_id, secrets, schema = get_data_id(size)
    schema_size: int = len(schema)

    # table情報と列指定情報を定義して計算
    table = [[data_id], [], [1]]
    inp = ([i+1 for i in range(schema_size)], [schema_size])
    with PrintTime("correl"):
        res = get_result(qmpc.correl(table, inp), limit=10000)
    assert (res["is_ok"])

    # 正しく計算されたか確認
    secrets_np = np.array(secrets)
    correl_matrix = np.corrcoef(secrets_np.transpose())
    true_val = correl_matrix[:schema_size-1, schema_size-1].transpose()
    for x, y in zip(res["results"][0], true_val):
        assert (math.isclose(x, y, abs_tol=0.1))


@pytest.mark.parametrize(
    ("size"), bench_size
)
def test_hjoin(size: int, get_data_id):
    data_id1, secrets1, schema1 = get_data_id(size, data_num=1)
    data_id2, secrets2, schema2 = get_data_id(size, data_num=2)

    # table情報を定義して計算
    table = [[data_id1, data_id2], [2], [1, 1]]
    with PrintTime("hjoin"):
        res = get_result(qmpc.get_join_table(table), limit=10000)
    assert (res["is_ok"])

    # 正しく計算されたか確認
    exe_schema = res["results"]["schema"]
    assert (schema1[1:] + schema2[1:] == exe_schema)

    exe_table = res["results"]["table"]
    exe_table_sorted = sorted(exe_table)
    true_table = []
    for r1, r2 in zip(secrets1, secrets2):
        true_table.append(r1[1:]+r2[1:])
    true_table_sorted = sorted(true_table)
    for r1, r2 in zip(true_table_sorted, exe_table_sorted):
        for x, y in zip(r1, r2):
            assert (math.isclose(x, y, abs_tol=0.1))
