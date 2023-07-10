
import glob
import os
import time

from quickmpc import QMPC, JobStatus
from quickmpc.qmpc_new import QMPC as QMPC_new


def get_endpoints():
    party_size = os.getenv("PARTY_SIZE")
    if party_size:
        try:
            endpoints = [
                f"http://{os.environ[f'PARTY{i}']}:50000" for i in range(1, int(party_size)+1)]
            return endpoints
        except KeyError:
            raise
    else:
        return [
            "http://localhost:50001",
            "http://localhost:50002",
            "http://localhost:50003",
        ]


qmpc: QMPC = QMPC(
    get_endpoints()
)
qmpc_new: QMPC_new = QMPC_new(
    get_endpoints()
)


def __try_get_computation_result(job_uuid, is_limit, path='./result'):
    res_status = qmpc.get_computation_status(job_uuid)
    all_completed = all([status == JobStatus.COMPLETED
                         for status in res_status["statuses"]])
    if not all_completed:
        if is_limit:
            raise
        return None

    if not os.path.isdir(path):
        os.mkdir(path)

    get_res = qmpc.get_computation_result(job_uuid, path)
    res = qmpc.restore(job_uuid, path)
    get_res["results"] = res
    # NOTE
    # 計算結果取得時には含まれていないが，job_uuidがmodelの取得に使われるので
    # このような処理をしている
    get_res["job_uuid"] = job_uuid
    return get_res


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
