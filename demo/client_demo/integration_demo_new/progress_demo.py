import logging

import quickmpc
from quickmpc.qmpc_new import QMPC

if __name__ == '__main__':
    """ Step 0. loggerの設定(任意) """
    logger = logging.getLogger(__name__)
    formatter = ("%(levelname)-5s | %(message)s")
    logging.basicConfig(level=logging.DEBUG, format=formatter)
    quickmpc.get_logger().setLevel(level=logging.DEBUG)

    """ Step 1. QMPCの設定 """
    qmpc: QMPC = QMPC(
        ["http://localhost:50001",
         "http://localhost:50002",
         "http://localhost:50003"]
    )

    """ Step 2. シェア送信 """
    data_dir: str = "../data/"
    df1 = qmpc.read_csv(f"{data_dir}/data1-1.csv", index_col="id")
    df2 = qmpc.read_csv(f"{data_dir}/data1-2.csv", index_col="id")
    sdf1 = qmpc.send_to(df1)
    sdf2 = qmpc.send_to(df2)

    """ Step 3. 進捗ログ付きでjoin"""
    df = sdf1.join(sdf2).to_data_frame(progress=True)
    logger.info(df)
