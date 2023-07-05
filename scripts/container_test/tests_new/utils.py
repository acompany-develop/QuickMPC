import pandas as pd
from quickmpc import JobStatus
from quickmpc.qmpc_new import QMPC
from quickmpc.qmpc_request import QMPCRequest
from quickmpc.share_data_frame import ShareDataFrame

from .container import Containers

""" 各種コンテナ """
__cc_names = [
    "dev_cc1",
    "dev_cc_envoy1",
    "dev_cc3",
    "dev_cc_envoy2",
    "dev_cc2",
    "dev_cc_envoy3",
]
__mc_names = [
    "dev_mc1",
    "dev_mc_envoy1",
    "dev_mc2",
    "dev_mc_envoy2",
    "dev_mc3",
    "dev_mc_envoy3",
]
__bts_names = [
    "dev_bts",
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
qmpc: QMPC = QMPC(QMPCRequest([
    "http://localhost:50001",
    "http://localhost:50002",
    "http://localhost:50003",
], 1, 1))

df = pd.DataFrame([[1, 2, 3], [4, 5, 6]], columns=["a", "b", "c"])
