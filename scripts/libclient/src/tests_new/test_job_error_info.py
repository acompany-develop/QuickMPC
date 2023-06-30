import pandas as pd
import pytest
from quickmpc.exception import QMPCJobError
from utils import qmpc_new


@pytest.mark.parametrize(
    ("column"),
    [
        # column index is outside left of range
        (0),

        # column index is outside right of range
        (3),
    ]
)
def test_job_error_info(column: int):
    sdf = qmpc_new.send_to(pd.DataFrame([[1, 2], [3, 4]],
                                        columns=["s1", "s2"]))

    err_info = None
    try:
        sdf.sum([column]).to_data_frame()
    except QMPCJobError as e:
        err_info = e.err_info

    assert (err_info is not None)
    assert (err_info.what != '')
    assert (err_info.HasField("stacktrace"))
