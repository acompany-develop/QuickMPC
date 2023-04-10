import pytest

from container import Containers
from utils import (all_containers, cc_p, data_id, execute_computation,
                   get_computation_result, job_uuid, mc_all, mc_p, send_share)


@pytest.mark.parametrize(
    ("down_container"), [
        (mc_p(1)),
    ]
)
def test_failed_send_share_with_down(down_container):
    # 特定のコンテナがdownした状態でsend_shareを送ってエラーが出るかどうか
    Containers.down_all()
    mc_all().up()

    # コンテナをdownさせてからsend_shareを送る
    down_container.down()
    res = send_share()

    Containers.down_all()
    assert not res["is_ok"]


@pytest.mark.parametrize(
    ("down_container"), [
        (mc_p(1)),
        (cc_p(1)),
    ]
)
def test_failed_execute_computation_with_down(down_container):
    # 特定のコンテナがdownした状態でexecute_requestを送ってエラーが出るかどうか
    Containers.down_all()
    all_containers().up()

    # コンテナを落とす前にsend_shareしておく
    data_id1: str = data_id()
    data_id2: str = data_id()

    # コンテナをdownさせてからexecute_computation(hjoin)を送る
    down_container.down()
    res = execute_computation(data_id1, data_id2)

    Containers.down_all()
    assert not res["is_ok"]


@pytest.mark.parametrize(
    ("down_container"), [
        (mc_p(1)),
    ]
)
def test_failed_get_computation_result_with_down(down_container):
    # 特定のコンテナがdownした状態でget_compuation_resultを送ってエラーが出るかどうか
    Containers.down_all()
    all_containers().up()

    # コンテナを落とす前にexecuteしておく
    job_uuid1: str = job_uuid()

    # コンテナをdownさせてからget_compuation_resultを送る
    down_container.down()
    res = get_computation_result(job_uuid1)

    Containers.down_all()
    assert not res["is_ok"]
