import time

import pytest

from utils import (all_containers, bts_p, cc_p, data_id, execute_computation,
                   get_computation_result, job_uuid, mc_all, mc_p, qmpc,
                   send_share)


@pytest.mark.parametrize(
    ("down_container"), [
        (mc_p(1)), (mc_p(2)), (mc_p(3)),
    ]
)
def test_failed_send_share_with_down(down_container):
    # 特定のコンテナがdownした状態でsend_shareを送ってエラーが出るかどうか
    all_containers().down()
    mc = mc_all()
    mc.up()

    # コンテナをdownさせてからsend_shareを送る
    down_container.down()
    res = send_share()
    mc.down()

    assert not res["is_ok"]


@pytest.mark.parametrize(
    ("down_container"), [
        (mc_p(1)),
        (cc_p(1)),
    ]
)
def test_failed_execute_computation_with_down(down_container):
    # 特定のコンテナがdownした状態でexecute_requestを送ってエラーが出るかどうか
    ac = all_containers()
    ac.down()
    ac.up()

    # コンテナを落とす前にsend_shareしておく
    data_id1: str = data_id()
    data_id2: str = data_id()

    # コンテナをdownさせてからexecute_computation(hjoin)を送る
    down_container.down()
    res = execute_computation(data_id1, data_id2)
    ac.down()

    assert not res["is_ok"]


@pytest.mark.parametrize(
    ("down_container"), [
        (mc_p(1)), (mc_p(2)), (mc_p(3)),
    ]
)
def test_failed_get_computation_result_with_down(down_container):
    # 特定のコンテナがdownした状態でget_compuation_resultを送ってエラーが出るかどうか
    ac = all_containers()
    ac.down()
    ac.up()

    # コンテナを落とす前にexecuteしておく
    job_uuid1: str = job_uuid()

    # コンテナをdownさせてからget_compuation_resultを送る
    down_container.down()
    res = get_computation_result(job_uuid1)
    ac.down()

    assert not res["is_ok"]
