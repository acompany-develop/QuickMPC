import os

import pytest
from quickmpc.exception import QMPCServerError
from quickmpc.qmpc import QMPC
from utils import data_frame, get_endpoints, qmpc

df = data_frame([[1, 2], [3, 4]], columns=["s1", "s2"])


# テスト用に正しいtokenで送信して得られたインスタンス
@pytest.fixture(scope="module")
def correct_sdf():
    return qmpc.send_to(df)


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
