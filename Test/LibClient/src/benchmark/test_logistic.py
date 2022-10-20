import time

import pytest
from sklearn.linear_model import LogisticRegression
from utils import get_result, qmpc

from benchmark.data import iris, penguins, titanic
from benchmark.metrics import MetricsOutputer


def logistic_sklearn(train_x, train_y, test_x):
    """ sklearnのロジスティック回帰 """
    lr = LogisticRegression()
    lr.fit(train_x, train_y)
    pred_y = lr.predict(test_x)
    return pred_y


def logistic_qmpc(train_x, train_y, test_x):
    """ QuickMPCのロジスティック回帰 """
    train_size: int = len(train_x)
    test_size: int = len(test_x)

    # 各データにIDの列を付与する
    ids_train = [f"{x}" for x in range(train_size)]
    ids_test = [f"{x}" for x in range(test_size)]
    schema_x = ["id"]+[f"x{i}" for i in range(len(train_x[0]))]

    train_x_include_id = [[i] + row for i, row in
                          zip(ids_train, train_x.tolist())]
    train_y_include_id = [[i] + row for i, row in
                          zip(ids_train, train_y.reshape(-1, 1).tolist())]
    test_x_include_id = [[i] + row for i, row in
                         zip(ids_test, test_x.tolist())]
    column_size: int = len(train_x_include_id[0])

    # データをシェア化し送信(schema,idは適当)
    res_train_x = qmpc.send_share(train_x_include_id, schema_x)
    assert (res_train_x["is_ok"])
    res_train_y = qmpc.send_share(train_y_include_id, ["id", "y"])
    assert (res_train_y["is_ok"])
    res_test_x = qmpc.send_share(test_x_include_id, schema_x)
    assert (res_test_x["is_ok"])
    id_train_x = res_train_x["data_id"]
    id_train_y = res_train_y["data_id"]
    id_test_x = res_test_x["data_id"]

    # ロジスティック回帰学習
    table_train = [[id_train_x, id_train_y], [0], [1, 1]]
    inp_train = [[i for i in range(2, column_size+1)], [column_size+1]]
    res_train = get_result(qmpc.logistic_regression(table_train, inp_train))
    assert (res_train["is_ok"])
    model_param_job_uuid: str = res_train["job_uuid"]

    # ロジスティック回帰推論
    table_pred = [[id_test_x], [], [1]]
    inp_pred = [i for i in range(2, column_size+1)]
    res_pred = get_result(qmpc.logistic_regression_predict(
        model_param_job_uuid, table_pred, inp_pred))
    assert (res_pred["is_ok"])

    # 冪等性のために消しておく
    qmpc.delete_share([id_train_x, id_train_y, id_test_x])

    # 分類なので01に整形して返す
    pred_y_num = res_pred["results"]
    pred_y_bin = [True if x > 0.5 else False for x in pred_y_num]
    return pred_y_bin


@pytest.mark.parametrize(
    ("train_size"),
    [
        (5), (10)
    ]
)
def test_logistic_titanic(train_size):
    # 現実時間で学習を終わらせるためサイズを小さくする
    train_x, train_y, test_x, test_y, _, _ = titanic()
    train_x = train_x[:train_size]
    train_y = train_y[:train_size]

    # trainデータで学習してtestデータでの推測値を得る
    pred_y_qmpc = logistic_qmpc(train_x, train_y, test_x)
    pred_y_sklearn = logistic_sklearn(train_x, train_y, test_x)

    # 指標を出力
    mo = MetricsOutputer("classification")
    mo.append("sklean", test_y, pred_y_sklearn)
    mo.append("QuickMPC", test_y, pred_y_qmpc)
    mo.output()


@pytest.mark.parametrize(
    ("train_size"),
    [
        (5), (10)
    ]
)
def test_logistic_iris(train_size):
    # 現実時間で学習を終わらせるためサイズを小さくする
    train_x, train_y_list, test_x, test_y_list, _, _ = iris()
    train_x = train_x[:train_size]
    train_y_list = train_y_list[:train_size]

    mo = MetricsOutputer("classification")
    labels = ["setosa", "versicolor", "virginica"]

    # trainデータで学習してtestデータでの推測値を得る
    for label, train_y, test_y in \
            zip(labels, train_y_list.transpose(), test_y_list.transpose()):
        pred_y_qmpc = logistic_qmpc(train_x, train_y, test_x)
        pred_y_sklearn = logistic_sklearn(train_x, train_y, test_x)

        mo.append(f"[{label}] sklean", test_y, pred_y_sklearn)
        mo.append(f"[{label}] QuickMPC", test_y, pred_y_qmpc)

    mo.output()


@pytest.mark.parametrize(
    ("train_size"),
    [
        (5), (10)
    ]
)
def test_logistic_penguins(train_size):
    # 現実時間で学習を終わらせるためサイズを小さくする
    train_x, train_y_list, test_x, test_y_list, _, _ = penguins()
    train_x = train_x[:train_size]
    train_y_list = train_y_list[:train_size]

    mo = MetricsOutputer("classification")
    labels = ['Adelie', 'Chinstrap', 'Gentoo']

    # trainデータで学習してtestデータでの推測値を得る
    for label, train_y, test_y in \
            zip(labels, train_y_list.transpose(), test_y_list.transpose()):
        pred_y_qmpc = logistic_qmpc(train_x, train_y, test_x)
        pred_y_sklearn = logistic_sklearn(train_x, train_y, test_x)

        mo.append(f"[{label}] sklean", test_y, pred_y_sklearn)
        mo.append(f"[{label}] QuickMPC", test_y, pred_y_qmpc)

    mo.output()
