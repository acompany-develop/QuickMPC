import logging

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

    # data_idに対して計算のリクエストを行う
    data_ids = \
        ["95ac225f2f9c4d72f95c85373fd7ade9d3c22520654f45be569ae9c06c801709"]
    res = qmpc.correl(
        (
            data_ids,  # 結合するdata_ids
            [],  # 結合方向(0:横，1:縦)
            [1]  # 各dataで結合に用いる列
        ),
        (
            [2, 3],  # 指定する列1
            [4]  # 指定する列2(func(列1, 列2)のように計算される)
        )
    )
    logger.info(res)
