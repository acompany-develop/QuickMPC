from container import Containers

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


def init():
    # 自分自身(container_test)をdownしないように1つずつ指定してdownさせる
    cc_all().down()
    mc_all().down()
    bts_p().down()
