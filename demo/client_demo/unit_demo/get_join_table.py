
import logging
import time

from quickmpc import QMPC

logger = logging.getLogger(__name__)
logging.basicConfig(level=logging.DEBUG)


def send_share(filename: str):
    res = qmpc.send_share_from_csv_file(filename)
    return res["data_id"]


if __name__ == '__main__':
    # QuickMPC Setting
    qmpc: QMPC = QMPC(
        ["http://localhost:50001",
         "http://localhost:50002",
         "http://localhost:50003"]
    )

    # 複数のシェアを作成・送信
    data_id1: str = send_share("../data/data1-1.csv")
    data_id2: str = send_share("../data/data1-2.csv")

    res = qmpc.get_join_table([data_id1, data_id2])

    job_uuid: str = res["job_uuid"]
    for _ in range(100):
        # 計算が終わるか100秒(1秒x100回)経つまで繰り返す
        get_res = qmpc.get_computation_result(job_uuid)
        if get_res["results"] is not None:
            res = get_res["results"]
            break
        time.sleep(1)
    logger.info(res)
