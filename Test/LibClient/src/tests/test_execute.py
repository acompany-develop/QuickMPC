import math
import random

import numpy as np
import pytest
from utils import get_result, qmpc


# tableデータを送信してdata_idとtable dataを得る
def send_share(filename: str) -> tuple:
    # csv dataをパースする
    secrets, schema = qmpc.parse_csv_file(filename)

    # データをシェア化し送信する
    send_res = qmpc.send_share(secrets, schema)
    assert(send_res["is_ok"])
    return secrets, send_res["data_id"]


@pytest.mark.parametrize(
    ("table_file"),
    [
        # NOTE: テスト時間が長くなるためテストケースを一部省略
        ("table_data_5x5"),
        # ("table_data_10x10"),
        # ("table_data_100x10"),
        # ("table_data_1000x10"),
        # ("table_data_10000x10"),
        ("table_data_100x100"),
    ]
)
def test_mean(table_file: str):

    # share送信
    secrets, data_id = send_share(f"Data/{table_file}.csv")

    # table情報と列指定情報を定義して計算
    length: int = len(secrets[0])
    table = [[data_id], [], [1]]
    inp = [i for i in range(2, length+1)]

    res = get_result(qmpc.mean(table, inp))
    assert(res["is_ok"])

    # 正しく計算されたか確認
    secrets_np = np.array(secrets)[:, 1:]
    true_val = np.add.reduce(secrets_np) / len(secrets)
    for x, y in zip(res["results"], true_val):
        assert(math.isclose(x, y, abs_tol=0.1))

    # 冪等性のために消しておく
    qmpc.delete_share([data_id])


@pytest.mark.parametrize(
    ("table_file"),
    [
        # NOTE: テスト時間が長くなるためテストケースを一部省略
        ("table_data_5x5"),
        # ("table_data_10x10"),
        # ("table_data_100x10"),
        # ("table_data_1000x10"),
        # ("table_data_10000x10"),
        ("table_data_100x100"),
    ]
)
def test_sum(table_file: str):

    # share送信
    secrets, data_id = send_share(f"Data/{table_file}.csv")

    # table情報と列指定情報を定義して計算
    length = len(secrets[0])
    table = [[data_id], [], [1]]
    inp = [i for i in range(2, length+1)]

    res = get_result(qmpc.sum(table, inp))
    assert(res["is_ok"])

    # 正しく計算されたか確認
    secrets_np = np.array(secrets)[:, 1:]
    true_val = np.add.reduce(secrets_np)
    for x, y in zip(res["results"], true_val):
        assert(math.isclose(x, y, abs_tol=0.1))

    # 冪等性のために消しておく
    qmpc.delete_share([data_id])


@pytest.mark.parametrize(
    ("table_file"),
    [
        # NOTE: テスト時間が長くなるためテストケースを一部省略
        ("table_data_5x5"),
        # ("table_data_10x10"),
        # ("table_data_100x10"),
        # ("table_data_1000x10"),
        # ("table_data_10000x10"),
        ("table_data_100x100"),
    ]
)
def test_variance(table_file: str):

    # share送信
    secrets, data_id = send_share(f"Data/{table_file}.csv")

    # table情報と列指定情報を定義して計算
    length = len(secrets[0])
    table = [[data_id], [], [1]]
    inp = [i for i in range(2, length+1)]

    res = get_result(qmpc.variance(table, inp))
    assert(res["is_ok"])

    # 正しく計算されたか確認
    secrets_np = np.array(secrets)[:, 1:]
    true_val = np.var(secrets_np, axis=0)
    for x, y in zip(res["results"], true_val):
        assert(math.isclose(x, y, abs_tol=0.1))

    # 冪等性のために消しておく
    qmpc.delete_share([data_id])


@pytest.mark.parametrize(
    ("table_file"),
    [
        # NOTE: テスト時間が長くなるためテストケースを一部省略
        ("table_data_5x5"),
        # ("table_data_10x10"),
        # ("table_data_100x10"),
        # ("table_data_1000x10"),
        # ("table_data_10000x10"),
        ("table_data_100x100"),
    ]
)
def test_correl(table_file: str):

    # share送信
    secrets, data_id = send_share(f"Data/{table_file}.csv")

    # table情報と列指定情報を定義して計算
    # 最後の列と各列との相関係数を計算
    length = len(secrets[0])
    table = [[data_id], [], [1]]
    inp = [[i for i in range(2, length)], [length]]

    res = get_result(qmpc.correl(table, inp))
    assert(res["is_ok"])

    # 正しく計算されたか確認
    secrets_np = np.array(secrets)[:, 1:]
    correl_matrix = np.corrcoef(secrets_np.transpose())
    true_val = correl_matrix[:length-2, length-2].transpose()

    for x, y in zip(res["results"][0], true_val):
        assert(math.isclose(x, y, abs_tol=0.1))

    # 冪等性のために消しておく
    qmpc.delete_share([data_id])


@pytest.mark.parametrize(
    ("table_file"),
    [
        # NOTE: 大きいケースは計算が遅いので省略
        ("table_data_5x5"),
        ("table_data_10x10"),
        # ("table_data_100x10"),
        # ("table_data_1000x10"),
        # ("table_data_10000x10"),
        # ("table_data_100x100"),
    ]
)
def test_linear_regression(table_file: str):
    # NOTE: 探索要素があるため，真値との比較はできない

    # share送信
    secrets, data_id = send_share(f"Data/{table_file}.csv")

    # table情報と列指定情報を定義して計算
    # 最後の列を目的値として計算
    length = len(secrets[0])
    table = [[data_id], [], [1]]
    inp = [[i for i in range(2, length)], [length]]

    res = get_result(qmpc.linear_regression(table, inp))
    assert(res["is_ok"])

    # 冪等性のために消しておく
    qmpc.delete_share([data_id])


@pytest.mark.parametrize(
    ("table_file"),
    [
        # NOTE: 大きいケースは計算が遅いので省略
        ("table_data_5x5"),
        ("table_data_10x10"),
        # ("table_data_100x10"),
        # ("table_data_1000x10"),
        # ("table_data_10000x10"),
        # ("table_data_100x100"),
    ]
)
def test_logistic_regression(table_file: str):
    # NOTE: 探索要素があるため，真値との比較はできない

    # share送信
    secrets, schema = qmpc.parse_csv_file(f"Data/{table_file}.csv")
    send_res = qmpc.send_share(secrets, schema)
    data_id = send_res["data_id"]

    # 目的値として01の値が必要なので用意する
    secrets01 = [[s[0]] + [0 if random.random() < 0.5 else 1] for s in secrets]
    send_res01 = qmpc.send_share(secrets01, ["attr01"])
    data_id01: str = send_res01["data_id"]

    # table情報と列指定情報を定義して計算
    # 最後の列を目的値として計算
    length = len(secrets[0])
    table = [[data_id, data_id01], [0], [1, 1]]
    inp = [[i for i in range(2, length)], [length+1]]

    res = get_result(qmpc.logistic_regression(table, inp))
    assert(res["is_ok"])


@pytest.mark.parametrize(
    ("table_file"),
    [
        ("table_data_5x5"),
    ]
)
def test_decision_tree(table_file: str):
    # NOTE: 探索要素があるため，真値との比較はできない

    # share送信
    secrets, schema = qmpc.parse_csv_file_to_bitvector(
        f"Data/{table_file}.csv")
    send_res = qmpc.send_share(secrets, schema)
    data_id = send_res["data_id"]

    # table情報と列指定情報を定義して計算
    # 最後の列を目的値として計算(bitvector化しているため2列分)
    table = [[data_id], [], [1]]
    inp = ([2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13], [19, 20])

    res = get_result(qmpc.decision_tree(table, inp))
    assert(res["is_ok"])


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
    assert(res["is_ok"])

    # 正しく計算されたか確認
    secrets_np = np.array(secrets)[:, 1:]
    true_val = np.add.reduce(secrets_np)
    for x, y in zip(res["results"], true_val):
        assert(math.isclose(x, y, abs_tol=0.1))

    # 冪等性のために消しておく
    qmpc.delete_share([data_id1, data_id2])


@pytest.mark.parametrize(
    ("table_file", "table_file_for_join"),
    [
        ("table_data_5x5", "table_data_5x5_for_join"),
        ("table_data_5x5", "table_data_5x5_for_failjoin"),
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
    assert(res["is_ok"])

    # 正しく計算されたか確認
    secrets_np = np.array(secrets)[:, 1:]
    true_val = np.add.reduce(secrets_np)
    for x, y in zip(res["results"], true_val):
        assert(math.isclose(x, y, abs_tol=0.1))

    # 冪等性のために消しておく
    qmpc.delete_share([data_id1, data_id2])


@pytest.mark.parametrize(
    ("table_file", "table_file_for_join"),
    [
        ("table_data_5x5", "table_data_5x5_for_join"),
        ("table_data_5x5", "table_data_5x5_for_failjoin"),
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
    assert(res["is_ok"])

    # 正しく計算されたか確認
    secrets_np = np.array(secrets)[:, 1:]
    true_val = np.add.reduce(secrets_np)
    for x, y in zip(res["results"], true_val):
        assert(math.isclose(x, y, abs_tol=0.1))

    # 冪等性のために消しておく
    qmpc.delete_share([data_id1, data_id2])
