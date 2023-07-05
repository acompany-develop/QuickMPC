import time

import pytest
from quickmpc import QMPC, JobErrorInfo, JobStatus
from quickmpc.exception import QMPCJobError
from utils import get_result, qmpc

from tests.common import data_id


def execute_computation_param(dataIds=[data_id([[1, 2, 3], [4, 5, 6]])],
                              src=[1, 2, 3]):
    return (dataIds, src)


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
    res = qmpc.sum(*param)
    job_uuid = res["job_uuid"]
    print()
    for retry in range(10):
        res = qmpc.get_computation_status(job_uuid)
        error = any([status == JobStatus.ERROR
                     for status in res["statuses"]])
        if error:
            res = qmpc.get_job_error_info(job_uuid)
            for info in res["job_error_info"]:
                if info:
                    err_info = info
            break
        time.sleep(5)

    assert (err_info is not None)
    assert (err_info.what != '')
    assert (err_info.HasField("stacktrace"))
