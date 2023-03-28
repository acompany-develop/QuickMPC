from quickmpc import QMPC

from container import Containers

qmpc: QMPC = QMPC([
    "http://localhost:50001",
    "http://localhost:50002",
    "http://localhost:50003",
])

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
