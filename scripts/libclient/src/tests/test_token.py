import csv
import os
import time

from quickmpc import QMPC, parse
from quickmpc.exception import QMPCServerError
from utils import get_result, qmpc

# 各種リクエストに用いる変数
filename_table: str = "data/table_data_5x5.csv"
secrets, schema = parse(filename_table)
length: int = len(secrets[0])
inp = [i for i in range(2, length + 1)]


def __preprocess():
    # 一部のrequestは別のrequestに依存しているため実行しておく
    res_send_share = qmpc.send_share_from_csv_file(filename_table)
    data_id: str = res_send_share["data_id"]

    res_execute_computation = qmpc.sum([data_id], inp)
    get_result(res_execute_computation)
    job_uuid: str = res_execute_computation["job_uuid"]

    return data_id, job_uuid


def check_all_request(token: str):
    os.environ["QMPC_TOKEN"] = token
    qmpc_inner: QMPC = QMPC(
        ["http://localhost:50001",
         "http://localhost:50002",
         "http://localhost:50003"],
    )
    data_id, job_uuid = __preprocess()

    # 各requestを実行する
    requests = [
        (qmpc_inner.send_share_from_csv_file, [filename_table]),
        (qmpc_inner.sum, ([data_id], inp)),
        (qmpc_inner.get_computation_result, (job_uuid,)),
        (qmpc_inner.delete_share, ([data_id],))
    ]

    def try_func(func, param):
        try:
            return func(*param)["is_ok"]
        except QMPCServerError:
            """tokenが正しくない場合例外が投げられるため"""
            return False

    response = [try_func(func, param) for func, param in requests]

    return response


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
