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
def test_get_elapsed_time(table_file: str):

    # share送信
    secrets, data_id = send_share(f"Data/{table_file}.csv")

    # table情報と列指定情報を定義して計算
    length = len(secrets[0])
    table = [[data_id], [], [1]]
    inp = [i for i in range(2, length+1)]

    response = qmpc.sum(table, inp)
    res = get_result(response)
    assert(res["is_ok"])
    job_uuid = response["job_uuid"]

    res = qmpc.get_elapsed_time(job_uuid)
    assert(res["is_ok"])

    # 冪等性のために消しておく
    qmpc.delete_share([data_id])
