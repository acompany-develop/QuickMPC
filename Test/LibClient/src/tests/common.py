
from utils import qmpc


def send_share(filename: str) -> tuple:
    # csv dataをパースする
    secrets, schema = qmpc.parse_csv_file(filename)

    # データをシェア化し送信する
    send_res = qmpc.send_share(secrets, schema)
    assert (send_res["is_ok"])
    return secrets, send_res["data_id"]


def data_id(secrets, schema=None):
    schema = schema if schema else ["s" for _ in range(len(secrets[0]))]
    res_send = qmpc.send_share(secrets, schema)
    assert (res_send["is_ok"])
    return res_send["data_id"]


def job_uuid(secrets, schema=None):
    res_send = qmpc.send_model_params(secrets)
    assert (res_send["is_ok"])
    return res_send["job_uuid"]
