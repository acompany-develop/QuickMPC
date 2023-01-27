
import logging
from typing import List

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

    # モデルパラメータ
    params_vec: List[float] = [1, 2, 3, 4]
    params_dict: List[dict] = [{"a": 1, "b": 2, "c": {"c1": 3, "c2": 4}}]

    # モデルパラメータを送信する
    res = qmpc.send_model_params(params_vec)
    logger.info(res)
    res = qmpc.send_model_params(params_dict)
    logger.info(res)
