import math

import pytest
from utils import get_result, qmpc

from tests.common import data_id


def execute_computation_param(dataIds=[data_id([[1, 2, 3], [4, 7, 10], [2, 9, 3]])],
                              join=[],
                              index=[1],
                              src=[1, 2],
                              target=[3]):
    return ((dataIds, join, index), (src, target))


@pytest.mark.parametrize(
    ("param", "expected"),
    [
        # usually case
        (execute_computation_param(),
         [[0.94491118, 0.24019223]]),

        # large data case
        (execute_computation_param(dataIds=[data_id([[10**9, 10**9+5],
                                                     [10**9+10, 10**9+10]])],
                                   src=[1],
                                   target=[2]),
            [[1, 1]]),

        # duplicated src, target case
        (execute_computation_param(src=[2, 3, 3],
                                   target=[1, 1, 3]),
         [[0.54470478, 0.94491118, 0.94491118],
          [0.54470478, 0.94491118, 0.94491118],
            [0.24019223, 1, 1]]),

        # empty src case
        (execute_computation_param(src=[]),
         [[]]),
    ]
)
def test_correl(param: tuple, expected: list):

    # 相関係数計算リクエスト送信
    res = get_result(qmpc.correl(*param))
    assert (res["is_ok"])

    # 正しく計算されたか確認
    for xl, yl in zip(res["results"], expected):
        for x, y in zip(xl, yl):
            assert (math.isclose(x, y, abs_tol=0.1))
