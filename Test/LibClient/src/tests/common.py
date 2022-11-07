
from utils import qmpc


def send_share(filename: str) -> tuple:
    # csv dataをパースする
    secrets, schema = qmpc.parse_csv_file(filename)

    # データをシェア化し送信する
    send_res = qmpc.send_share(secrets, schema)
    assert (send_res["is_ok"])
    return secrets, send_res["data_id"]
