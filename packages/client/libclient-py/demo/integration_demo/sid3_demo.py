""" QuickMPCによるSID3の動作デモ
Shareをbitvectorにして送信する必要があるため別ファイルとしている．
Step 1. QuickMPC動作のための準備
Step 2. シェアをbitvector化してコンテナに送信
Step 3. 計算リクエストを送信
Step 4. シェアを元に復元された演算結果の真値を取得
"""

import logging
import time

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
    secrets1, schema1 = qmpc.parse_csv_file_to_bitvector(
        data_dir+"data1-1.csv")
    secrets2, schema2 = qmpc.parse_csv_file_to_bitvector(
        data_dir+"data1-2.csv")

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

    """ Step 3. data_idに対して計算のリクエスト送信 """
    data_ids = [data_id1, data_id2]
    inp = ([2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13], [19, 20])

    logger.info("---- decision_tree start ----")
    exec_res = qmpc.decision_tree((data_ids, [0], [1, 1]), inp)

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
            logger.info("---- decision_tree success ----")
            break

        statuses = make_statuses_detailed(get_res['statuses'])
        logger.info(f"statuses: {statuses}")

        time.sleep(1)
