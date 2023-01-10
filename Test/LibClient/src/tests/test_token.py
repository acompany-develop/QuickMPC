
import csv
import time

from quickmpc import QMPC
from quickmpc.exception import QMPCServerError
from utils import get_result, qmpc

# 各種リクエストに用いる変数
filename_table: str = "Data/table_data_5x5.csv"
secrets, schema = qmpc.parse_csv_file(filename_table)
length: int = len(secrets[0])
filename_model: str = "Data/model_data_a6.csv"
inp = [i for i in range(2, length + 1)]
data = []
with open(filename_model) as f:
    reader = csv.reader(f, quoting=csv.QUOTE_NONNUMERIC)
    data = [row for row in reader][0]


def __preprocess():
    # 一部のrequestは別のrequestに依存しているため実行しておく
    res_send_share = qmpc.send_share(secrets, schema)
    data_id: str = res_send_share["data_id"]

    table = [[data_id], [], [1]]
    res_execute_computation = qmpc.sum(table, inp)
    get_result(res_execute_computation)
    job_uuid: str = res_execute_computation["job_uuid"]

    res_send_model_params = qmpc.send_model_params(data)
    model_param_job_uuid: str = res_send_model_params["job_uuid"]

    return data_id, job_uuid, model_param_job_uuid


def check_all_request(token: str):
    qmpc_inner: QMPC = QMPC(
        ["http://localhost:9001",
         "http://localhost:9002",
         "http://localhost:9003"],
        token
    )
    data_id, job_uuid, model_param_job_uuid = __preprocess()
    table = [[data_id], [], [1]]

    # 各requestを実行する
    try:
        res_send_share = qmpc_inner.send_share(secrets, schema)
    except QMPCServerError:
        """tokenが正しくない場合例外が投げられるため"""
        res_send_share = {"is_ok":False}

    try:
        res_execute_computation = qmpc_inner.sum(table, inp)
    except QMPCServerError:
        res_execute_computation = {"is_ok":False}

    try:
        res_get_computation_result = qmpc_inner.get_computation_result(job_uuid)
    except QMPCServerError:
        res_get_computation_result = {"is_ok":False}

    try:
        res_send_model_params = qmpc_inner.send_model_params(data)
    except QMPCServerError:
        res_send_model_params = {"is_ok":False}

    try:
        res_predict = qmpc_inner.linear_regression_predict(
            model_param_job_uuid, table, inp)
    except QMPCServerError:
        res_predict = {"is_ok":False}

    try:
        res_delete_share = qmpc_inner.delete_share([data_id])
    except QMPCServerError:
        res_delete_share = {"is_ok":False}

    return [res_send_share["is_ok"],
            res_execute_computation["is_ok"],
            res_get_computation_result["is_ok"],
            res_send_model_params["is_ok"],
            res_predict["is_ok"],
            res_delete_share["is_ok"]]


def test_token_dep():
    """ deploy tokenでの動作確認 """
    token: str = "token_dep"
    res = check_all_request(token)
    assert (all(res))


def test_token_demo():
    """ demo tokenでの動作確認 """
    token: str = "token_demo"
    res = check_all_request(token)
    assert (all(res))


def test_token_error():
    """ 適当なtokenでの動作確認 """
    token: str = "token_hoge"
    res = check_all_request(token)
    assert (any(res) is False)
