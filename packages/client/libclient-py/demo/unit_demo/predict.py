import logging
from typing import List, Tuple

from quickmpc import QMPC

logger = logging.getLogger(__name__)
logging.basicConfig(level=logging.DEBUG)

if __name__ == '__main__':
    # QuickMPC Setting
    qmpc: QMPC = QMPC(
        ["http://localhost:9001",
         "http://localhost:9002",
         "http://localhost:9003"]
    )

    # モデルの予測値を計算して取得する
    model_param_job_uuid: str = "73063ce1-8827-431d-8a7d-428d63bb144b"
    model_id: int = 1
    data_ids = \
        ["95ac225f2f9c4d72f95c85373fd7ade9d3c22520654f45be569ae9c06c801709"]
    table: Tuple[List, List, List] = (data_ids, [], [1])

    res = qmpc.linear_regression_predict(
        model_param_job_uuid,  # モデルパラメータのID
        table,  # 予測に使用するデータ(data_idと結合方向)
        [2, 3]  # 予測に使用するデータの列指定
    )
    logger.info(res)
