""" QuickMPCからのレスポンスに含まれる計算処理の進捗を表示するデモ
本デモコードは次の操作を行っている．
Step 1. QuickMPC動作のための準備
Step 2. 送信用データの生成
Step 3. シェアをコンテナに送信
Step 4. 計算リクエストを送信
Step 5. レスポンスからプログレスバーを作成・更新
"""

import logging
import time
import collections
from typing import List, Tuple, OrderedDict, Optional

import tqdm

from quickmpc import QMPC, JobStatus

logger = logging.getLogger(__name__)
formatter = ("%(levelname)-5s | %(message)s")
logging.basicConfig(level=logging.DEBUG, format=formatter)


def large_data(size: int, data_num: int):
    schema_size: int = 10
    schema: List[str] = ["id"] + \
        [f"s{i}_{data_num}" for i in range(schema_size - 1)]

    mod = 47
    data: List[List[str]] = [
        [str(i)] + [str(((i * size + k) % mod) / mod)
                    for k in range(schema_size - 1)]
        for i in range(size)
    ]
    return qmpc.parse_csv_data([schema] + data)


def get_data_id(size: int, data_num):
    """ Step 2. 送信用データの生成 """
    secrets, schema = large_data(size, data_num)

    """ Step 3. シェアをコンテナに送信 """
    res = qmpc.send_share(secrets, schema)
    assert (res["is_ok"])
    data_id: str = res["data_id"]

    return (data_id, secrets, schema)


if __name__ == '__main__':

    """ Step 1. 諸準備 """
    # QuickMPC Setting
    qmpc: QMPC = QMPC(
        ["http://localhost:9001",
         "http://localhost:9002",
         "http://localhost:9003"]
    )
    # データ Setting
    size = 50000
    data_id1, secrets1, schema1 = get_data_id(size, data_num=1)
    data_id2, secrets2, schema2 = get_data_id(size, data_num=2)

    """ Step 4. テーブル横結合をリクエスト"""
    table = ([data_id1, data_id2], [2], [1, 1])
    exec_res = qmpc.get_join_table(table)

    job_uuid = exec_res["job_uuid"]
    logger.info(f"job_uuid: {job_uuid}")

    pbars: OrderedDict[Tuple[int, int],
                       Tuple[Optional[tqdm.tqdm], float]] \
        = collections.OrderedDict()

    QMPC.set_log_level(logging.WARN)

    while True:
        get_res = qmpc.get_computation_result(job_uuid)

        """ Step 5. プログレスバーの作成・更新 """
        if get_res['statuses'] is not None:
            for party_id, status in enumerate(get_res['statuses']):
                key = (party_id, -1)

                if key not in pbars:
                    pbars[(party_id, -1)] = (tqdm.tqdm(
                        desc=f"[{party_id}] status",
                        total=len(JobStatus.items()) - 1), 0)

                pbar, prev = pbars[key]
                if pbar is None:
                    continue

                pbar.update(status - prev)
                pbar.set_postfix(
                    status=JobStatus.Name(status)
                )

                if status == JobStatus.Value('COMPLETED'):
                    pbar.close()
                    pbar = None

                pbars[key] = (pbar, status)

        progresses = get_res['progresses']
        if progresses is not None:
            for party_id, progress in enumerate(progresses):
                if progress is None:
                    continue

                for procedure in progress.progresses:
                    key = (party_id, procedure.id)

                    if key not in pbars:
                        pbars[key] = (tqdm.tqdm(
                            desc=f"[{party_id}] {procedure.description}",
                            total=100), 0)

                    pbar, prev = pbars[key]
                    if pbar is None:
                        continue

                    pbar.update(procedure.progress - prev)
                    pbar.set_postfix(details=procedure.details)

                    if procedure.completed:
                        pbar.close()
                        pbar = None

                    pbars[key] = (pbar, procedure.progress)

        if get_res["results"] is not None:
            for key in pbars:
                pbar, prev = pbars[key]
                if pbar is None:
                    continue
                pbar.update(100 - prev)
                pbar.clear()
                pbar.close()

            break

        time.sleep(1)
