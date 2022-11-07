import math

import pytest
from utils import get_result, qmpc

from tests.common import data_id


def execute_computation_param(dataIds=[data_id([[1, 2, 3], [4, 5, 6]])],
                              join=[],
                              index=[1],
                              src=[1, 2],
                              target=[3]):
    return ((dataIds, join, index), (src, target))


@pytest.mark.parametrize(
    ("param"),
    [
        # usually case
        (execute_computation_param()),
        # large data case
        (execute_computation_param(dataIds=[data_id([[10**9, 10**9+5],
                                                     [10**9+10, 10**9+10]])],
                                   src=[1],
                                   target=[2])),
        # duplicated src, target case
        (execute_computation_param(src=[2, 3, 3],
                                   target=[1, 1, 3])),
        # empty src case
        (execute_computation_param(src=[])),
    ]
)
def test_linear_regression(param: tuple):

    # 線形回帰リクエスト送信
    res = get_result(qmpc.linear_regression(*param))
    assert (res["is_ok"])
