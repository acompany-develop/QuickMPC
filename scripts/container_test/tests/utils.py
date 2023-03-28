import time

from quickmpc import QMPC, JobStatus

from container import Containers

""" 各種コンテナ """
__cc_names = [
    "dev_unit_cc1",
    "dev_unit_cc_envoy1",
    "dev_unit_cc3",
    "dev_unit_cc_envoy2",
    "dev_unit_cc2",
    "dev_unit_cc_envoy3",
]
__mc_names = [
    "dev_unit_mc1",
    "dev_unit_mc_envoy1",
    "dev_unit_mc2",
    "dev_unit_mc_envoy2",
    "dev_unit_mc3",
    "dev_unit_mc_envoy3",
]
__bts_names = [
    "dev_unit_bts",
]


def cc_p(n: int) -> Containers:
    # 1-indexでparty-nのCCを返す
    return Containers(__cc_names[2*n-2:2*n])


def cc_all() -> Containers:
    return Containers(__cc_names)


def mc_p(n: int) -> Containers:
    # 1-indexでparty-nのMCを返す
    return Containers(__mc_names[2*n-2:2*n])


def mc_all() -> Containers:
    return Containers(__mc_names)


def bts_p() -> Containers:
    return Containers(__bts_names)


def all_containers() -> Containers:
    return Containers(__cc_names+__mc_names+__bts_names)


""" quickmpcへ簡易request群 """
qmpc: QMPC = QMPC([
    "http://localhost:50001",
    "http://localhost:50002",
    "http://localhost:50003",
])


def send_share() -> dict:
    return qmpc.send_share([[1, 2, 3], [4, 5, 6]], ["a", "b", "c"])


def data_id() -> str:
    res_ss = send_share()
    assert res_ss["is_ok"]
    return res_ss["data_id"]


def execute_computation(data_id1: str, data_id2: str) -> dict:
    return qmpc.get_join_table(([data_id1, data_id2], [2], [1, 1]))


def job_uuid() -> str:
    res_ec = execute_computation(data_id(), data_id())
    assert res_ec["is_ok"]
    job_uuid_: str = res_ec["job_uuid"]
    # 計算結果が保存されるまで待機する
    for _ in range(10):
        try:
            res = qmpc.get_computation_result(job_uuid)
        except Exception:
            continue
        if res["statuses"] is not None:
            if all([status == JobStatus.COMPLETED
                    for status in res["statuses"]]):
                return job_uuid_
        time.sleep(3)
    raise RuntimeError("`execute` is not completed.")


def get_computation_result(job_uuid: str) -> dict:
    return qmpc.get_computation_result(job_uuid)
