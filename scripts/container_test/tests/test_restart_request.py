import pytest

from tests.container import Containers
from tests.utils import all_containers, bts_p, cc_p, df, mc_all, mc_p, qmpc


@pytest.mark.parametrize(
    ("restart_container"), [
        (mc_p(1)),
    ]
)
def test_success_send_share_with_restart(restart_container):
    # 特定のコンテナをrestartさせてsend_shareを送れるか
    Containers.down_all()
    mc_all().up()

    # コンテナをrestartさせてからsend_shareを送る
    restart_container.restart()
    qmpc.send_to(df)


@pytest.mark.parametrize(
    ("restart_container"), [
        (mc_p(1)),
        (cc_p(1)),
    ]
)
def test_success_execute_computation_with_restart(restart_container):
    # 特定のコンテナをrestartさせてexecute_computationを送れるか
    Containers.down_all()
    all_containers().up()

    # コンテナを落とす前にsend_shareしておく
    sdf: str = qmpc.send_to(df)

    # コンテナをrestartさせてからexecute_computationを送る
    restart_container.restart()
    sdf.join(sdf)


@pytest.mark.parametrize(
    ("restart_container"), [
        (mc_p(1)),
    ]
)
def test_success_get_computation_result_with_restart(restart_container):
    # 特定のコンテナをrestartさせてget_compuation_resultを送れるか
    Containers.down_all()
    all_containers().up()

    # コンテナを落とす前にexecuteしておく
    sdf = qmpc.send_to(df)
    sdf_join = sdf.join(sdf)
    sdf_join._wait_execute(progress=False)

    # コンテナをrestartさせてからget_compuation_resultを送る
    restart_container.restart()
    sdf_join.to_data_frame()


@pytest.mark.parametrize(
    ("restart_container"), [
        (mc_p(1)), (cc_p(1)), (bts_p()),
    ]
)
def test_success_execute_multiple_with_restart(restart_container):
    # 特定のコンテナをrestartさせてhjoinを何回も計算できるか
    Containers.down_all()
    all_containers().up()

    for _ in range(3):
        sdf: str = qmpc.send_to(df)
        sdf.join(sdf)._wait_execute(progress=False)
        restart_container.restart()
