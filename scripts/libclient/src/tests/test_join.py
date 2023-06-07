import math

import pytest
from utils import get_result, qmpc

from tests.common import data_id


def execute_computation_param(dataIds=[data_id([[1000, 1, 2], [1001, 1, 2]],
                                               ["id", "s1", "s2"]),
                                       data_id([[1000, 1, 2], [1002, 1, 2]],
                                               ["id", "s1", "s3"])]):

    return (dataIds, )


@pytest.mark.parametrize(
    ("param", "expected"),
    [
        # not share join case
        (execute_computation_param(),
         [[1, 2, 1, 2]]),

        # all match case
        (execute_computation_param(dataIds=[data_id([[1000, 1, 2], [1001, 3, 4]],
                                                    ["id", "s1", "s2"]),
                                            data_id([[1000, 1, 2], [1001, 3, 4]],
                                                    ["id", "s3", "s4"])]),
            [[1, 2, 1, 2], [3, 4, 3, 4]]),

        # not match case
        (execute_computation_param(dataIds=[data_id([[1000, 1, 2]], ["id", "s1", "s2"]),
                                            data_id([[1001, 1, 2]], ["id", "s3", "s4"])]),
            [[]]),

        # empty table case
        (execute_computation_param(dataIds=[data_id([[1000]], ["id"]),
                                            data_id([[1000]], ["id"])]),
         [[]]),

        # large table case
        (execute_computation_param(dataIds=[data_id([[id, 1] for id in range(1000)], ["id", "s1"]),
                                            data_id([[id, 2] for id in range(1000)], ["id", "s2"])], ),
         [[1, 2]for _ in range(1000)]),
    ]
)
def test_hjoin(param: tuple, expected: list):

    # 非Shareの横結合リクエスト送信
    res = get_result(qmpc.get_join_table(*param, debug_mode=True))
    assert (res["is_ok"])

    # 正しく計算されたか確認
    for xl, yl in zip(res["results"]["table"], expected):
        for x, y in zip(xl, yl):
            assert (math.isclose(x, y, abs_tol=0.1))


@pytest.mark.parametrize(
    ("param", "expected"),
    [
        # not share join case
        (execute_computation_param(),
         [[1, 2, 1, 2]]),

        # all match case
        (execute_computation_param(dataIds=[data_id([[1000, 1, 2], [1001, 3, 4]],
                                                    ["id", "s1", "s2"]),
                                            data_id([[1000, 1, 2], [1001, 3, 4]],
                                                    ["id", "s3", "s4"])]),
            [[1, 2, 1, 2], [3, 4, 3, 4]]),

        # not match case
        (execute_computation_param(dataIds=[data_id([[1000, 1, 2]], ["id", "s1", "s2"]),
                                            data_id([[1001, 1, 2]], ["id", "s3", "s4"])]),
            [[]]),

        # empty table case
        (execute_computation_param(dataIds=[data_id([[1000]], ["id"]),
                                            data_id([[1000]], ["id"])]),
         [[]]),

        # large table case
        (execute_computation_param(dataIds=[data_id([[id, 1] for id in range(1000)], ["id", "s1"]),
                                            data_id([[id, 2] for id in range(1000)], ["id", "s2"])], ),
         [[1, 2]for _ in range(1000)]),
    ]
)
def test_hjoin_share(param: tuple, expected: list):

    # Shareの横結合リクエスト送信
    res = get_result(qmpc.get_join_table(*param))
    assert (res["is_ok"])

    # 正しく計算されたか確認
    for xl, yl in zip(res["results"]["table"], expected):
        for x, y in zip(xl, yl):
            assert (math.isclose(x, y, abs_tol=0.1))
