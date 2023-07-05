import pytest
from quickmpc.exception import QMPCServerError

from .container import Containers
from .utils import all_containers, cc_p, df, mc_all, mc_p, qmpc


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
    with pytest.raises(QMPCServerError):
        qmpc.send_to(df)


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
    sdf: str = qmpc.send_to(df)

    # コンテナをdownさせてからexecute_computation(hjoin)を送る
    down_container.down()
    with pytest.raises(QMPCServerError):
        sdf.join(sdf)


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
    sdf: str = qmpc.send_to(df)
    sdf_join = sdf.join(sdf)
    sdf_join._wait_execute(progress=False)

    # コンテナをdownさせてからget_compuation_resultを送る
    down_container.down()
    with pytest.raises(QMPCServerError):
        sdf_join.to_data_frame()
