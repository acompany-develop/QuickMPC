import math

import pytest
from utils import get_result, qmpc

from tests.common import data_id


def execute_computation_param(dataIds=[data_id([[1, 2, 3], [4, 5, 6]])],
                              src=[1, 2, 3]):
    return (dataIds, src)


@pytest.mark.parametrize(
    ("param", "expected"),
    [
        # usually case
        (execute_computation_param(),
         [5, 7, 9]),

        # small table size case
        (execute_computation_param(dataIds=[data_id([[1]])],
                                   src=[1]),
         [1]),

        # large data case
        (execute_computation_param(dataIds=[data_id([[10**18], [10**18]])],
                                   src=[1]),
            [2*10**18]),

        # small data case
        (execute_computation_param(dataIds=[data_id([[10**-10], [10**-10]])],
                                   src=[1]),
            [2*10**-10]),

        # duplicated src case
        (execute_computation_param(src=[1, 2, 2, 3, 3, 3]),
         [5, 7, 7, 9, 9, 9]),

        # empty src case
        (execute_computation_param(src=[]),
         []),
    ]
)
def test_sum(param: tuple, expected: list):

    # 総和計算リクエスト送信
    res = get_result(qmpc.sum(*param))
    assert (res["is_ok"])

    # 正しく計算されたか確認
    for x, y in zip(res["results"], expected):
        assert (math.isclose(x, y, abs_tol=0.1))
