import math

import numpy as np
import pytest
from utils import get_result, qmpc

from .common import send_share


@pytest.mark.parametrize(
    ("table_file", "table_file_for_join"),
    [
        ("table_data_5x5", "table_data_5x5_for_join"),
    ]
)
def test_table_hjoin(table_file: str, table_file_for_join: str):

    # share送信
    secrets1, data_id1 = send_share(f"Data/{table_file}.csv")
    secrets2, data_id2 = send_share(f"Data/{table_file_for_join}.csv")
    secrets = [r1 + r2[4:] for r1, r2 in zip(secrets1, secrets2)][:3]

    # table情報と列指定情報を定義して計算
    length: int = len(secrets[0])
    table = [[data_id1, data_id2], [0], [1, 1]]
    inp = [i for i in range(2, length+1)]

    res = get_result(qmpc.sum(table, inp))
    assert (res["is_ok"])

    # 正しく計算されたか確認
    secrets_np = np.array(secrets)[:, 1:]
    true_val = np.add.reduce(secrets_np)
    for x, y in zip(res["results"], true_val):
        assert (math.isclose(x, y, abs_tol=0.1))

    # 冪等性のために消しておく
    qmpc.delete_share([data_id1, data_id2])


@pytest.mark.parametrize(
    ("table_file", "table_file_for_join"),
    [
        ("table_data_5x5", "table_data_5x5_for_join"),
    ]
)
def test_table_wjoin(table_file: str, table_file_for_join: str):

    # share送信
    secrets1, data_id1 = send_share(f"Data/{table_file}.csv")
    secrets2, data_id2 = send_share(f"Data/{table_file_for_join}.csv")
    secrets = [r[:4] for r in secrets1] + [r[:4] for r in secrets2[3:]]

    # table情報と列指定情報を定義して計算
    length: int = len(secrets[0])
    table = [[data_id1, data_id2], [1], [1, 1]]
    inp = [i for i in range(2, length+1)]

    res = get_result(qmpc.sum(table, inp))
    assert (res["is_ok"])

    # 正しく計算されたか確認
    secrets_np = np.array(secrets)[:, 1:]
    true_val = np.add.reduce(secrets_np)
    for x, y in zip(res["results"], true_val):
        assert (math.isclose(x, y, abs_tol=0.1))

    # 冪等性のために消しておく
    qmpc.delete_share([data_id1, data_id2])


@pytest.mark.parametrize(
    ("table_file", "table_file_for_join"),
    [
        ("table_data_5x5", "table_data_5x5_for_join"),
    ]
)
def test_table_hjoin_share(table_file: str, table_file_for_join: str):

    # share送信
    secrets1, data_id1 = send_share(f"Data/{table_file}.csv")
    secrets2, data_id2 = send_share(f"Data/{table_file_for_join}.csv")
    secrets = [r1 + r2[4:] for r1, r2 in zip(secrets1, secrets2)][:3]

    # table情報と列指定情報を定義して計算
    length: int = len(secrets[0])
    table = [[data_id1, data_id2], [2], [1, 1]]
    inp = [i for i in range(2, length+1)]

    res = get_result(qmpc.sum(table, inp))
    assert (res["is_ok"])

    # 正しく計算されたか確認
    secrets_np = np.array(secrets)[:, 1:]
    true_val = np.add.reduce(secrets_np)
    for x, y in zip(res["results"], true_val):
        assert (math.isclose(x, y, abs_tol=0.1))

    # 冪等性のために消しておく
    qmpc.delete_share([data_id1, data_id2])


@pytest.mark.parametrize(
    ("table_file", "table_file_for_join"),
    [
        ("table_data_5x5", "table_data_5x5_for_failjoin"),
    ]
)
def test_table_hjoin_fail(table_file: str, table_file_for_join: str):

    # share送信
    secrets1, data_id1 = send_share(f"Data/{table_file}.csv")
    secrets2, data_id2 = send_share(f"Data/{table_file_for_join}.csv")

    # table情報を定義してテーブル結合リクエスト送信
    table = [[data_id1, data_id2], [0], [1, 1]]
    res = get_result(qmpc.get_join_table(table))

    # 結合結果が空でもエラーなくresponseが返ってくるか
    assert (res["is_ok"])


@pytest.mark.parametrize(
    ("table_file", "table_file_for_join"),
    [
        ("table_data_5x5", "table_data_5x5_for_failjoin"),
    ]
)
def test_table_wjoin_fail(table_file: str, table_file_for_join: str):

    # share送信
    secrets1, data_id1 = send_share(f"Data/{table_file}.csv")
    secrets2, data_id2 = send_share(f"Data/{table_file_for_join}.csv")

    # table情報を定義してテーブル結合リクエスト送信
    table = [[data_id1, data_id2], [1], [1, 1]]
    res = get_result(qmpc.get_join_table(table))

    # 結合結果が空でもエラーなくresponseが返ってくるか
    assert (res["is_ok"])


@pytest.mark.parametrize(
    ("table_file", "table_file_for_join"),
    [
        ("table_data_5x5", "table_data_5x5_for_failjoin"),
    ]
)
def test_table_hjoin_share_fail(table_file: str, table_file_for_join: str):

    # share送信
    secrets1, data_id1 = send_share(f"Data/{table_file}.csv")
    secrets2, data_id2 = send_share(f"Data/{table_file_for_join}.csv")

    # table情報を定義してテーブル結合リクエスト送信
    table = [[data_id1, data_id2], [2], [1, 1]]
    res = get_result(qmpc.get_join_table(table))

    # 結合結果が空でもエラーなくresponseが返ってくるか
    assert (res["is_ok"])
