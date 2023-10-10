import pandas as pd
import pytest
from utils import data_frame, qmpc


@pytest.mark.parametrize(
    ("df", "columns", "expected"),
    [
        # usually case
        (data_frame([[1, 2, 3], [4, 5, 6]], columns=["s1", "s2", "s3"]),
         [1, 2, 3],
         pd.DataFrame([5.0, 7.0, 9.0])),

        # small table size case
        (data_frame([[1]], columns=["s1"]),
         [1],
         pd.DataFrame([1.0])),

        # large data case
        (data_frame([[10**18], [10**18]], columns=["s1"]),
         [1],
         pd.DataFrame([2.0*10**18])),

        # small data case
        (data_frame([[10**-8], [10**-8]], columns=["s1"]),
         [1],
         pd.DataFrame([2.0*10**-8])),

        # duplicated src case
        (data_frame([[1, 2, 3], [4, 5, 6]], columns=["s1", "s2", "s3"]),
         [1, 2, 2, 3, 3, 3],
         pd.DataFrame([5.0, 7.0, 7.0, 9.0, 9.0, 9.0])),
    ]
)
def test_sum(df: pd.DataFrame, columns: list, expected: pd.DataFrame):
    sdf = qmpc.send_to(df)
    result = sdf.sum(columns).to_data_frame()
    pd.testing.assert_frame_equal(result, expected, atol=1e-7)
