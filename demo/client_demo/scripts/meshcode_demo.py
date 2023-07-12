import logging

import pandas as pd
import quickmpc
import quickmpc.pandas as qpd

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
    df: pd.DataFrame = qpd.read_csv(f"{data_dir}/data-meshcode.csv",
                                    index_col="id")
    sdf: qpd.ShareDataFrame = qmpc.send_to(df)

    """ Step 3. 各種統計演算 """
    logger.info(sdf.meshcode([2, 3, 4, 5]).to_data_frame())
