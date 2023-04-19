
from utils import qmpc


def data_id(secrets, schema=None):
    schema = schema if schema else [f"s{i}" for i in range(len(secrets[0]))]
    data = [schema] + secrets
    res_send = qmpc.send_share_from_csv_data(data)
    assert (res_send["is_ok"])
    return res_send["data_id"]
