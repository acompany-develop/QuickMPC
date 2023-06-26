import math
import time
import os

import pytest
from utils import get_result, qmpc

from tests.common import data_id
from quickmpc import JobStatus

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
    ]
)
def test_get_computation_result(param: tuple, expected: list):
    # 総和計算リクエスト送信
    res = qmpc.sum(*param)
    assert (res["is_ok"])
    for i in range(10000):
        time.sleep(i+1)
        get_res = qmpc.get_computation_result(res["job_uuid"])
        assert (get_res["is_ok"])

        if get_res["results"] is None:
            continue
        # 正しく取得できたか確認
        for x, y in zip(get_res["results"], expected):
            assert (math.isclose(x, y, abs_tol=0.1))
        break


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
    ]
)
def test_restore(param: tuple, expected: list):
    path = "./result"
    if not os.path.isdir(path):
        os.mkdir(path)

    res = qmpc.sum(*param)
    assert (res["is_ok"])
    for i in range(10000):
        time.sleep(i+1)
        get_res = qmpc.get_computation_result(res["job_uuid"], path)
        assert (get_res["is_ok"])

        all_completed = all([status == JobStatus.COMPLETED
                                for status in get_res["statuses"]])

        if not all_completed:
            continue
        res = qmpc.restore(res["job_uuid"], path)
        # 正しく取得できたか確認
        for x, y in zip(res, expected):
            assert (math.isclose(x, y, abs_tol=0.1))
        break
