

import logging

from quickmpc import QMPC

logger = logging.getLogger(__name__)
logging.basicConfig(level=logging.DEBUG)

if __name__ == '__main__':
    # QuickMPC Setting
    qmpc: QMPC = QMPC(
        ["http://localhost:50001",
         "http://localhost:50002",
         "http://localhost:50003"]
    )

    # 複数のシェアを作成・送信
    data_ids = []
    for i in range(3):
        res = qmpc.send_share([[1]], ["key"])
        data_ids.append(res['data_id'])
        logger.info(res)

    # シェアを削除
    for i in range(2):
        res = qmpc.delete_share(data_ids)
        logger.info(res)
        # 2 回目は失敗することを確認
        assert res["is_ok"] == (i == 0)
