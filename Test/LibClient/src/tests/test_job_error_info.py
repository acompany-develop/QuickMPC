import grpc
from grpc_status import rpc_status
from quickmpc import JobErrorInfo

import pytest
from utils import get_result, qmpc

from tests.common import data_id


def execute_computation_param(dataIds=[data_id([[1, 2, 3], [4, 5, 6]])],
                              join=[],
                              index=[1],
                              src=[1, 2, 3]):
    return ((dataIds, join, index), src)


@pytest.mark.parametrize(
    ("param"),
    [
        # column index is outside left of range
        execute_computation_param(src=[0]),

        # column index is outside right of range
        execute_computation_param(src=[4]),
    ]
)
def test_job_error_info(param: tuple):
    err_info = None
    try:
        # 総和計算リクエスト送信
        get_result(qmpc.sum(*param))
    except grpc.RpcError as e:
        # re-throw される例外について調べる
        status = rpc_status.from_call(e)
        if status is not None:
            for detail in status.details:
                if detail.Is(
                    JobErrorInfo.DESCRIPTOR
                ):
                    err_info = JobErrorInfo()
                    detail.Unpack(err_info)

    assert (err_info is not None)
    assert (err_info.what != '')
    assert (err_info.HasField("stacktrace"))
