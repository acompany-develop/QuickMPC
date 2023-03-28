import time

import pytest
from quickmpc import JobStatus

from utils import all_containers, bts_p, cc_p, mc_all, mc_p, qmpc


def __send_share() -> dict:
    return qmpc.send_share([[1, 2, 3], [4, 5, 6]], ["a", "b", "c"])


def __data_id() -> str:
    res_ss = __send_share()
    assert res_ss["is_ok"]
    return res_ss["data_id"]


def __execute_computation(data_id1: str, data_id2: str) -> dict:
    return qmpc.get_join_table(([data_id1, data_id2], [2], [1, 1]))


def __job_uuid() -> str:
    res_ec = __execute_computation(__data_id(), __data_id())
    assert res_ec["is_ok"]
    job_uuid: str = res_ec["job_uuid"]
    # 計算結果が保存されるまで待機する
    for _ in range(10):
        try:
            res = qmpc.get_computation_result(job_uuid)
        except Exception:
            continue
        if res["statuses"] is not None:
            if all([status == JobStatus.COMPLETED
                    for status in res["statuses"]]):
                return job_uuid
        time.sleep(3)
    raise RuntimeError("`execute` is not completed.")


def __get_computation_result(job_uuid: str) -> dict:
    return qmpc.get_computation_result(job_uuid)


@pytest.mark.parametrize(
    ("down_container"), [
        (mc_p(1))
    ]
)
def test_failed_send_share_with_down(down_container):
    # 特定のコンテナがdownした状態でsend_shareを送ってエラーが出るかどうか
    all_containers().down()
    mc = mc_all()
    mc.up()

    # コンテナをdownさせてからsend_shareを送る
    down_container.down()
    res = __send_share()
    mc.down()

    assert not res["is_ok"]


@pytest.mark.parametrize(
    ("down_container"), [
        (cc_p(1))
    ]
)
def test_failed_execute_computations_with_down(down_container):
    # 特定のコンテナがdownした状態でexecute_requestを送ってエラーが出るかどうか
    ac = all_containers()
    ac.down()
    ac.up()

    # コンテナを落とす前にsend_shareしておく
    data_id1: str = __data_id()
    data_id2: str = __data_id()

    # コンテナをdownさせてからexecute_computation(hjoin)を送る
    down_container.down()
    res = __execute_computation(data_id1, data_id2)
    ac.down()

    assert not res["is_ok"]


@pytest.mark.parametrize(
    ("down_container"), [
        (mc_p(1))
    ]
)
def test_failed_get_computation_result_with_down(down_container):
    # 特定のコンテナがdownした状態でget_compuation_resultを送ってエラーが出るかどうか
    ac = all_containers()
    ac.down()
    ac.up()

    # コンテナを落とす前にexecuteしておく
    job_uuid: str = __job_uuid()

    # コンテナをdownさせてからget_compuation_resultを送る
    down_container.down()
    res = __get_computation_result(job_uuid)
    ac.down()

    assert not res["is_ok"]
