""" QuickMPCによる演算の動作デモ
本デモコードは次の操作を行っている．
Step 1. QuickMPC動作のための準備
Step 2. シェアをコンテナに送信
Step 3. 計算リクエストを送信
Step 4. シェアを元に復元された演算結果の真値を取得
"""

import logging
import time
from typing import Callable, List, Tuple

from utils import make_statuses_detailed

from quickmpc import QMPC

logger = logging.getLogger(__name__)
formatter = ("%(levelname)-5s | %(message)s")
logging.basicConfig(level=logging.DEBUG, format=formatter)

if __name__ == '__main__':

    """ Step 1. 諸準備 """
    # QuickMPC Setting
    qmpc: QMPC = QMPC(
        ["http://localhost:9001",
         "http://localhost:9002",
         "http://localhost:9003"]
    )
    # データ Setting
    data_dir: str = "../data/"
    secrets1, schema1 = qmpc.parse_csv_file(data_dir+"data1-1.csv")
    secrets2, schema2 = qmpc.parse_csv_file(data_dir+"data1-2.csv")
    secrets3, schema3 = qmpc.parse_csv_file(data_dir+"data-meshcode.csv")

    """ Step 2. シェア送信 """
    logger.info("---- send_share start ----")
    send_res1 = qmpc.send_share(secrets1, schema1)
    if not send_res1["is_ok"]:
        raise
    data_id1 = send_res1["data_id"]
    logger.info(f"data_id1: {data_id1}")

    time.sleep(1)
    send_res2 = qmpc.send_share(secrets2, schema2)
    if not send_res2["is_ok"]:
        raise
    data_id2 = send_res2["data_id"]
    logger.info(f"data_id2: {data_id2}")

    time.sleep(1)
    send_res3 = qmpc.send_share(secrets3, schema3)
    if not send_res3["is_ok"]:
        raise
    data_id3 = send_res3["data_id"]
    logger.info(f"data_id3: {data_id3}")
    logger.info("---- send_share success ----")

    """ Step 3. data_idに対して計算のリクエスト送信 """
    time.sleep(3)
    data_ids = [data_id1, data_id2, data_id3]
    # Jobを追加する場合はここに追加する
    # 形式: [呼び出すJob，指定する列，真値]
    exe_set: List[Tuple[Callable, List, List]] = [
        (qmpc.sum, [2, 3, 4], [12, 12, 17]),
        (qmpc.mean, [2, 3, 4], [2.4, 2.4, 3.4]),
        (qmpc.variance, [2, 3, 4], [2.24, 1.84, 4.24]),
        (
            qmpc.meshcode,
            [7, 8, 9, 10],
            [
                [53, 39, 4, 5, 4, 7, 1, 1],
                [65, 42, 5, 2, 1, 8, 3, 2],
                [47, 30, 3, 4, 0, 3, 2, 4],
                [51, 8, 4, 7, 4, 0, 3, 1],
            ]
        ),
        (qmpc.correl, [[2, 3, 4], [5]],
         [0.995537307016568, -0.192854233809008, 0.0441893193071959]),
        (qmpc.linear_regression, [[2, 3, 4], [5]],
         [-3.9106383, 12.81276596, 4.74468085, -3.30212766]),
        (qmpc.logistic_regression, [[2, 3, 4], [6]],
         [5.44821378, -0.67505836, -0.30945274, -0.72291515])]
    for exe_func, inp, true_val in exe_set:
        logger.info(f"---- {exe_func.__name__} start ----")
        # TODO: Jobごとに使用するdata_idを選択できるようにする
        exec_res = exe_func([data_ids, [0, 0], [1, 1, 1]], inp) \
            if exe_func == qmpc.meshcode  \
            else exe_func([data_ids[:2], [0], [1, 1]], inp)

        """ Step 4. 結果を取得 """
        job_uuid = exec_res["job_uuid"]
        logger.info(f"job_uuid: {job_uuid}")
        # DBに計算結果が書き込まれていないと思うので1秒待機
        time.sleep(1)

        for _ in range(100):
            # 計算が終わるか100秒(1秒x100回)経つまで繰り返す
            get_res = qmpc.get_computation_result(job_uuid)
            if get_res["results"] is not None:
                result_secrets = get_res["results"]
                logger.info(f"exec value: {result_secrets}")
                logger.info(f"true value: {true_val}")

                logger.info(f"---- {exe_func.__name__} success ----")
                break

            statuses = make_statuses_detailed(get_res['statuses'])
            logger.info(f"statuses: {statuses}")

            time.sleep(1)
