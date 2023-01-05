import math

import pytest
from utils import get_result, qmpc

from tests.common import data_id, job_uuid

usually_tree = [{
    "weight": 1.0,
    "tree_structure": {
        "feature_bitvector": [0, 1, 0],
        "threshold": 0.5,
        "right_child": {
            "output": 0
        },
        "left_child": {
            "output": 1
        }
    }
}]


def predict_param(model_param_job_uuid=job_uuid([1]),
                  dataIds=[data_id([[1, 1, 0], [1, 0, 1]],
                                   ["a#0", "b#0", "b#1"])],
                  join=[],
                  index=[1],
                  src=[1, 2, 3]):
    return (model_param_job_uuid, (dataIds, join, index), src)


@pytest.mark.skip(reason="廃止予定のためskip")
@pytest.mark.parametrize(
    ("param", "expected"),
    [
        # usually case
        (predict_param(),
         [0, 1]),
    ]
)
def test_decision_tree_predict(param: tuple, expected: list):

    # 決定木推論リクエスト送信
    res = get_result(qmpc.decision_tree_predict(*param))
    assert (res["is_ok"])

    # 正しく計算されたか確認
    for x, y in zip(res["results"], expected):
        assert (math.isclose(x, y, abs_tol=0.1))
