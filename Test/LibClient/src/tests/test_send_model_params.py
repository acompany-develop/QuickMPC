import csv
import json

import pytest
from utils import qmpc


def send_model_params_param(params=[1, 2, 3],
                            piece_size: int = 1_000_000):
    return (params, piece_size)


@pytest.mark.parametrize(
    ("param"),
    [
        # usually case
        (send_model_params_param()),
        # json case
        (send_model_params_param(params={"a": 1, "b": {"c": 2}})),
        # empty array case
        # TODO: fix ValueError
        # (send_model_params_param(params=[])),
        # empty json case
        (send_model_params_param(params={})),
        # small piece_size case
        (send_model_params_param(piece_size=1_000)),
    ]
)
def test_send_share(param: tuple):

    # モデルデータをシェア化し送信する
    res = qmpc.send_model_params(*param)
    assert (res["is_ok"])
