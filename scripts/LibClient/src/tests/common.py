
from utils import qmpc


def data_id(secrets, schema=None):
    schema = schema if schema else [f"s{i}" for i in range(len(secrets[0]))]
    res_send = qmpc.send_share(secrets, schema)
    assert (res_send["is_ok"])
    return res_send["data_id"]
