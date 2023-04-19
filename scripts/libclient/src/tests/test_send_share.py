
import csv
import json

import pytest
from utils import qmpc


def csv_data_param(secrets=[[1, 2, 3], [4, 5, 6]],
                   schema=["a", "b", "c"],
                   matching_column=1,
                   piece_size=1_000_000):
    return ([schema]+secrets, matching_column, piece_size)


@pytest.mark.parametrize(
    ("param"),
    [
        # usually case
        (csv_data_param()),

        # small table size case
        (csv_data_param(secrets=[[1]], schema=["a"])),

        # nonzero matching_column case
        (csv_data_param(matching_column=3)),

        # small piece_size case
        (csv_data_param(piece_size=1_000)),
    ]
)
def test_send_share_from_csv_data(param: tuple):

    # データをシェア化し送信する
    res = qmpc.send_share_from_csv_data(*param)
    assert (res["is_ok"])

    # 冪等性のために消しておく
    qmpc.delete_share([res["data_id"]])
