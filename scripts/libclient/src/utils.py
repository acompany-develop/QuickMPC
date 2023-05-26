
import time

from quickmpc import QMPC, JobStatus

qmpc: QMPC = QMPC([
    "http://localhost:50001",
    "http://localhost:50002",
    "http://localhost:50003",
])

def __try_get_computation_result(job_uuid, is_limit):
    try:
        get_res = qmpc.get_computation_result(job_uuid)
    except:
        if is_limit:
            raise
        return None


    all_completed = False
    if get_res["statuses"] is not None:
        all_completed = all([status == JobStatus.COMPLETED
                                for status in get_res["statuses"]])

    if all_completed:
        # NOTE
        # 計算結果取得時には含まれていないが，job_uuidがmodelの取得に使われるので
        # このような処理をしている
        get_res["job_uuid"] = job_uuid
        return get_res

    return None

def get_result(res, limit=20):
    """ 計算が終わるまで結果取得をし続ける(1秒x20回) """
    if not res["is_ok"]:
        return res

    job_uuid = res["job_uuid"]

    for i in range(limit):
        res = __try_get_computation_result(job_uuid, i+1 == limit)
        if res is not None:
            return res
        time.sleep(1)
