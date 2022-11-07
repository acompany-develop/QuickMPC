import math

import pytest
from utils import get_result, qmpc

from tests.common import data_id


def execute_computation_param(dataIds=[data_id([[1, 1, 0], [1, 0, 1]],
                                               ["a#0", "b#0", "b#1"])],
                              join=[],
                              index=[1],
                              src=[1],
                              target=[2, 3]):
    return ((dataIds, join, index), (src, target))


@pytest.mark.parametrize(
    ("param"),
    [
        # usually case
        (execute_computation_param()),
        # duplicated src, target case
        (execute_computation_param(src=[1, 2, 3, 2, 3],
                                   target=[1, 1, 2, 3])),
        # empty src case
        (execute_computation_param(src=[])),
    ]
)
def test_decision_tree(param: tuple):

    # 決定木リクエスト送信
    res = get_result(qmpc.decision_tree(*param))
    assert (res["is_ok"])
