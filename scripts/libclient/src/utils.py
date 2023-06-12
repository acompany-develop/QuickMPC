
import time
import glob
import os
from quickmpc import QMPC, JobStatus

qmpc: QMPC = QMPC([
    "http://localhost:50001",
    "http://localhost:50002",
    "http://localhost:50003",
])

def __try_get_computation_result(job_uuid, is_limit, path = './result'):
    try:
        if not os.path.isdir(path):
            os.mkdir(path)
        get_res = qmpc.get_computation_result(job_uuid, path)
    except:
        if is_limit:
            raise
        return None


    all_completed = False
    if get_res["statuses"] is not None:
        all_completed = all([status == JobStatus.COMPLETED
                                for status in get_res["statuses"]])

    if glob.glob(f"{path}/schema*{job_uuid}-*") == 0:
        return None
    if all_completed:
        res = qmpc.restore(job_uuid, path)
        print(job_uuid)
        get_res["results"] = res
        # NOTE
        # 計算結果取得時には含まれていないが，job_uuidがmodelの取得に使われるので
        # このような処理をしている
        get_res["job_uuid"] = job_uuid
        return get_res

    return None

def get_result(res, limit=20):
    """ 計算が終わるまで limit 回結果取得をし続ける i回目の終わりにi秒待つ"""
    if not res["is_ok"]:
        return res

    job_uuid = res["job_uuid"]

    for i in range(limit):
        time.sleep(i)
        res = __try_get_computation_result(job_uuid, i+1 == limit)
        if res is not None:
            return res
