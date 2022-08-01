
import numpy as np
import pytest
from utils import get_result, qmpc

from benchmark.data import iris, penguins, titanic
from benchmark.metrics import MetricsOutputer


def id3_sklearn(train_x, train_y, test_x):
    """ ID3 """
    # TODO: 何らかのライブラリのID3で学習させる
    return []


def sid3_qmpc(train_x, train_y, test_x, schema_x, schema_y):
    """ QuickMPCのSID3 """
    train_size: int = len(train_x)
    test_size: int = len(test_x)

    # 各データにIDの列を付与する
    ids_train = [f"{x}" for x in range(train_size)]
    ids_test = [f"{x}" for x in range(test_size)]

    train_x_include_id = [[i] + row for i, row in
                          zip(ids_train, train_x.tolist())]
    train_y_include_id = [[i] + row for i, row in
                          zip(ids_train, train_y.tolist())]
    test_x_include_id = [[i] + row for i, row in
                         zip(ids_test, test_x.tolist())]
    column_size: int = len(train_x_include_id[0])

    # データをシェア化し送信(idsは適当)
    res_train_x = qmpc.send_share(train_x_include_id, ["id#0"] + schema_x)
    assert(res_train_x["is_ok"])
    res_train_y = qmpc.send_share(train_y_include_id, ["id#0"] + schema_y)
    assert(res_train_y["is_ok"])
    res_test_x = qmpc.send_share(test_x_include_id, ["id#0"] + schema_x)
    assert(res_test_x["is_ok"])
    id_train_x = res_train_x["data_id"]
    id_train_y = res_train_y["data_id"]
    id_test_x = res_test_x["data_id"]

    # SID3学習
    table_train = [[id_train_x, id_train_y], [0], [1, 1]]
    inp_train = [[i for i in range(2, column_size + 1)],
                 [column_size + 1 + i for i in range(len(train_y[0]))]]
    res_train = get_result(qmpc.decision_tree(table_train, inp_train))
    assert(res_train["is_ok"])
    model_param_job_uuid: str = res_train["job_uuid"]

    # SID3推論
    table_pred = [[id_test_x], [], [1]]
    inp_pred = [i for i in range(2, column_size + 1)]
    res_pred = get_result(qmpc.sid3_tree_predict(model_param_job_uuid, table_pred, inp_pred))
    assert(res_pred["is_ok"])

    # 冪等性のために消しておく
    qmpc.delete_share([id_train_x, id_train_y, id_test_x])

    # 分類なので離散値に整形して返す
    pred_y_num = res_pred["results"]
    pred_y_class = [0 if y < 0.5 else 1 if y < 1.5 else 2 for y in pred_y_num]
    return pred_y_class


def convert(test_y):
    """ one hot vectorを元のデータに変換 """
    return [np.argmax(v) for v in test_y]


@pytest.mark.parametrize(
    ("train_size"),
    [
        (5), (10)
    ]
)
def test_sid3_titanic(train_size):
    # 現実時間で学習を終わらせるためサイズを小さくする
    train_x, train_y, test_x, test_y, schema_x, schema_y = titanic(
        is_bitvector=True)
    train_x = train_x[:train_size]
    train_y = train_y[:train_size]

    # trainデータで学習してtestデータでの推測値を得る
    pred_y_qmpc = sid3_qmpc(train_x, train_y, test_x, schema_x, schema_y)

    # 指標を出力
    mo = MetricsOutputer("classification")
    mo.append("QuickMPC", convert(test_y), pred_y_qmpc)
    mo.output()


@pytest.mark.parametrize(
    ("train_size"),
    [
        (5), (10)
    ]
)
def test_sid3_iris(train_size):
    # 現実時間で学習を終わらせるためサイズを小さくする
    train_x, train_y, test_x, test_y, schema_x, schema_y = iris(
        is_bitvector=True)
    train_x = train_x[:train_size]
    train_y = train_y[:train_size]

    # trainデータで学習してtestデータでの推測値を得る
    pred_y_qmpc = sid3_qmpc(train_x, train_y, test_x, schema_x, schema_y)

    # 指標を出力
    mo = MetricsOutputer("classification")
    mo.append("QuickMPC", convert(test_y), pred_y_qmpc)
    mo.output()


@pytest.mark.parametrize(
    ("train_size"),
    [
        (5), (10)
    ]
)
def test_sid3_penguins(train_size):
    # 現実時間で学習を終わらせるためサイズを小さくする
    train_x, train_y, test_x, test_y, schema_x, schema_y = penguins(
        is_bitvector=True)
    train_x = train_x[:train_size]
    train_y = train_y[:train_size]

    # trainデータで学習してtestデータでの推測値を得る
    pred_y_qmpc = sid3_qmpc(train_x, train_y, test_x, schema_x, schema_y)

    # 指標を出力
    mo = MetricsOutputer("classification")
    mo.append("QuickMPC", convert(test_y), pred_y_qmpc)
    mo.output()
