import math

import pytest
from utils import get_result, qmpc

from tests.common import data_id, job_uuid


def predict_param(model_param_job_uuid=job_uuid([1, 2, 3, 4]),
                  dataIds=[data_id([[1, 2, 3], [4, 5, 6]])],
                  join=[],
                  index=[1],
                  src=[1, 2, 3]):
    return (model_param_job_uuid, (dataIds, join, index), src)


@pytest.mark.parametrize(
    ("param", "expected"),
    [
        # usually case
        (predict_param(), [21, 48]),
        # large data case
        (predict_param(job_uuid([10**18, 10**9, 10**9]),
                       dataIds=[data_id([[10**9, 10**9+5],
                                         [10**9+10, 10**9+10]])],
                       src=[1, 2]),
         [3*10**18+5*10**9, 3*10**18+20*10**9]),
        # duplicated src case
        (predict_param(job_uuid([1, 2, 3, 4, 5, 6, 7]),
                       src=[1, 2, 2, 3, 3, 3]),
         [71, 152]),
        # param is 0 case
        (predict_param(job_uuid([0, 0, 0, 0])),
         [0, 0]),
        # empty src case
        # TODO: 空のsrcで動作するようにする(or エラーを出す)
        # (predict_param(src=[]),
        #  []),
    ]
)
def test_linear_regression_predict(param: tuple, expected: list):

    # 線形回帰推論送信
    res = get_result(qmpc.linear_regression_predict(*param))
    assert (res["is_ok"])

    # 正しく計算されたか確認
    for x, y in zip(res["results"], expected):
        assert (math.isclose(x, y, abs_tol=0.1))
