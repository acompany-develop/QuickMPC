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

    # data_idに対して計算のリクエストを行う
    data_ids = [
        "cedf69accf961f4074bd684227e7b18dbae92c3b78ee67bd5d56c8c27241ffd1",
        "36af53bfa7bd6a19ad3fcee8531353f365bd7462f0cc3bde6979710282bb469e"
    ]
    res = qmpc.correl(
        data_ids,  # 結合するdata_ids
        (
            [2, 3],  # 指定する列1
            [4]  # 指定する列2(func(列1, 列2)のように計算される)
        )
    )
    logger.info(res)
