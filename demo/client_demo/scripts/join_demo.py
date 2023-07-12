import logging

import pandas as pd
import quickmpc

if __name__ == '__main__':
    """ Step 0. loggerの設定(任意) """
    logger = logging.getLogger(__name__)
    formatter = ("%(levelname)-5s | %(message)s")
    logging.basicConfig(level=logging.DEBUG, format=formatter)
    quickmpc.get_logger().setLevel(level=logging.DEBUG)

    """ Step 1. QMPCの設定 """
    qmpc: quickmpc.QMPC = quickmpc.QMPC(
        ["http://localhost:50001",
         "http://localhost:50002",
         "http://localhost:50003"]
    )

    """ Step 2. シェア送信 """
    data_dir: str = "../data/"
    df1: pd.DataFrame = qmpc.read_csv(f"{data_dir}/data-join1.csv",
                                      index_col="id")
    df2: pd.DataFrame = qmpc.read_csv(f"{data_dir}/data-join2.csv",
                                      index_col="id")
    sdf1: quickmpc.ShareDataFrame = qmpc.send_to(df1)
    sdf2: quickmpc.ShareDataFrame = qmpc.send_to(df2)

    """ Step 3. join"""
    logger.info(sdf1.join(sdf2).to_data_frame())
