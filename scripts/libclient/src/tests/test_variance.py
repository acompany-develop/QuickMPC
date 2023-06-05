import math

import pytest
from utils import get_result, qmpc

from tests.common import data_id


def execute_computation_param(dataIds=[data_id([[1, 2, 3], [4, 7, 10]])],
                              join=[],
                              src=[1, 2, 3]):
    return ((dataIds, join), src)


@pytest.mark.parametrize(
    ("param", "expected"),
    [
        # usually case
        (execute_computation_param(),
         [2.25, 6.25, 12.25]),

        # small table size case
        (execute_computation_param(dataIds=[data_id([[1]])],
                                   src=[1]),
         [0]),

        # large data case
        (execute_computation_param(dataIds=[data_id([[10**9], [10**9+10]])],
                                   src=[1]),
            [25]),

        # small data case
        (execute_computation_param(dataIds=[data_id([[10**-10], [10**-10+10]])],
                                   src=[1]),
            [25]),

        # duplicated src case
        (execute_computation_param(src=[1, 2, 2, 3, 3, 3]),
         [2.25, 6.25, 6.25, 12.25, 12.25, 12.25]),

        # empty src case
        (execute_computation_param(src=[]),
         []),

        # variance is 0 case
        (execute_computation_param(dataIds=[data_id([[1], [1], [1]])],
                                   src=[1]),
         [0]),
    ]
)
def test_variance(param: tuple, expected: list):

    # 分散計算リクエスト送信
    res = get_result(qmpc.variance(*param))
    assert (res["is_ok"])

    # 正しく計算されたか確認
    for x, y in zip(res["results"], expected):
        assert (math.isclose(x, y, abs_tol=0.1))
