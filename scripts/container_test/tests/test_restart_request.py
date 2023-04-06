import pytest

from container import Containers
from utils import (all_containers, bts_p, cc_p, data_id, execute_computation,
                   get_computation_result, job_uuid, mc_all, mc_p, send_share)


@pytest.mark.parametrize(
    ("restart_container"), [
        (mc_p(1)),
    ]
)
def test_succes_send_share_with_restart(restart_container):
    # 特定のコンテナをrestartさせてsend_shareを送れるか
    Containers.down_all()

    mc = mc_all()
    mc.up()

    # コンテナをrestartさせてからsend_shareを送る
    restart_container.restart()
    res = send_share()

    Containers.down_all()
    assert res["is_ok"]


@pytest.mark.parametrize(
    ("restart_container"), [
        (mc_p(1)),
        (cc_p(1)),
    ]
)
def test_success_execute_computation_with_restart(restart_container):
    # 特定のコンテナをrestartさせてexecute_computationを送れるか
    Containers.down_all()

    ac = all_containers()
    ac.up()

    # コンテナを落とす前にsend_shareしておく
    data_id1: str = data_id()
    data_id2: str = data_id()

    # コンテナをrestartさせてからexecute_computationを送る
    restart_container.restart()
    res = execute_computation(data_id1, data_id2)

    Containers.down_all()
    assert res["is_ok"]


@pytest.mark.parametrize(
    ("restart_container"), [
        (mc_p(1)), (cc_p(1)),
    ]
)
def test_success_execute_computations_with_restart(restart_container):
    # 特定のコンテナをrestartさせてexecute_computationを送れるか
    Containers.down_all()

    ac = all_containers()
    ac.up()

    # コンテナを落とす前にsend_shareしておく
    data_id1: str = data_id()
    data_id2: str = data_id()

    # コンテナをrestartさせてからexecute_computationを送る
    restart_container.restart()
    res = execute_computation(data_id1, data_id2)

    Containers.down_all()
    assert res["is_ok"]


@pytest.mark.parametrize(
    ("restart_container"), [
        (mc_p(1)),
    ]
)
def test_success_get_computation_result_with_restart(restart_container):
    # 特定のコンテナをrestartさせてget_compuation_resultを送れるか
    Containers.down_all()

    ac = all_containers()
    ac.up()

    # コンテナを落とす前にexecuteしておく
    job_uuid1: str = job_uuid()

    # コンテナをrestartさせてからget_compuation_resultを送る
    restart_container.restart()
    res = get_computation_result(job_uuid1)

    Containers.down_all()
    assert res["is_ok"]


@pytest.mark.parametrize(
    ("restart_container"), [
        (mc_p(1)), (cc_p(1)), (bts_p()),
    ]
)
def test_success_execute_multiple_with_restart(restart_container):
    # 特定のコンテナをrestartさせてhjoinを何回も計算できるか
    Containers.down_all()

    ac = all_containers()
    ac.up()

    for _ in range(3):
        job_uuid()
        restart_container.restart()

    Containers.down_all()
