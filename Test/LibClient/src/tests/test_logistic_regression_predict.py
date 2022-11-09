import math

import pytest
from utils import get_result, qmpc

from tests.common import data_id, job_uuid


def predict_param(model_param_job_uuid=job_uuid([0.01, 0.01, 0.05, 0.03]),
                  dataIds=[data_id([[1, 2, 3], [4, 5, 6]])],
                  join=[],
                  index=[1],
                  src=[1, 2, 3]):
    return (model_param_job_uuid, (dataIds, join, index), src)


@pytest.mark.parametrize(
    ("param", "expected"),
    [
        # usually case
        (predict_param(),
         [0.5523079095743253, 0.617747874769249]),

        # large data case
        (predict_param(job_uuid([10**18, 10**9, 10**9]),
                       dataIds=[data_id([[10**9, 10**9+5],
                                         [10**9+10, 10**9+10]])],
                       src=[1, 2]),
         [1.0, 1.0]),

        # duplicated src case
        (predict_param(job_uuid([0.01, 0.02, 0.03, 0.04, 0.05, 0.06, 0.07]),
                       src=[1, 2, 2, 3, 3, 3]),
         [0.6704011598088686, 0.8205384805926733]),

        # param is 0 case
        (predict_param(job_uuid([0, 0, 0, 0])),
         [0.5, 0.5]),

        # empty src case
        # TODO: 空のsrcで動作するようにする(or エラーを出す)
        # (predict_param(src=[]),
        #  []),
    ]
)
def test_logistic_regression_predict(param: tuple, expected: list):

    # ロジスティック回帰推論送信
    res = get_result(qmpc.logistic_regression_predict(*param))
    assert (res["is_ok"])

    # 正しく計算されたか確認
    for x, y in zip(res["results"], expected):
        assert (math.isclose(x, y, abs_tol=0.1))
