import os

import pandas as pd
import pytest
from quickmpc.exception import QMPCServerError
from quickmpc.qmpc_new import QMPC
from utils import get_endpoints, qmpc_new

df = pd.DataFrame([[1, 2], [3, 4]], columns=["s1", "s2"])


# テスト用に正しいtokenで送信して得られたインスタンス
@pytest.fixture(scope="module")
def correct_sdf():
    return qmpc_new.send_to(df)


@pytest.fixture(scope="module")
def correct_result(correct_sdf):
    return correct_sdf.sum([1, 2])


@pytest.mark.parametrize(
    ("token"), [
        ("token_demo"),
        ("token_dep"),
    ]
)
def test_token_success(token: str, correct_sdf, correct_result):
    os.environ["QMPC_TOKEN"] = token
    qmpc_token = QMPC(get_endpoints())
    qmpc_token.send_to(df)
    correct_sdf.sum([1, 2])
    correct_result.to_data_frame()


@pytest.mark.parametrize(
    ("token"), [
        ("token_hoge"),
    ]
)
def test_token_error(token: str, correct_sdf, correct_result):
    os.environ["QMPC_TOKEN"] = token
    qmpc_token = QMPC(get_endpoints())
    with pytest.raises(QMPCServerError):
        qmpc_token.send_to(df)
    # TODO: 中でraiseするように変更する
    correct_sdf.sum([1, 2])
    correct_result.to_data_frame()
