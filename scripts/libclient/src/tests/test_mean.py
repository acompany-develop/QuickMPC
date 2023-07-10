import pandas as pd
import pytest
from utils import qmpc


@pytest.mark.parametrize(
    ("df", "columns", "expected"),
    [
        # usually case
        (pd.DataFrame([[1, 2, 3], [4, 5, 6]], columns=["s1", "s2", "s3"]),
         [1, 2, 3],
         pd.DataFrame([2.5, 3.5, 4.5])),

        # small table size case
        (pd.DataFrame([[1]], columns=["s1"]),
         [1],
         pd.DataFrame([1.0])),

        # large data case
        (pd.DataFrame([[10**18], [10**18+10]], columns=["s1"]),
         [1],
         pd.DataFrame([10.0**18+5])),

        # small data case
        (pd.DataFrame([[10**-10], [10**-10+10]], columns=["s1"]),
         [1],
         pd.DataFrame([10.0**-10+5])),

        # duplicated src case
        (pd.DataFrame([[1, 2, 3], [4, 5, 6]], columns=["s1", "s2", "s3"]),
         [1, 2, 2, 3, 3, 3],
         pd.DataFrame([2.5, 3.5, 3.5, 4.5, 4.5, 4.5])),
    ]
)
def test_mean(df: pd.DataFrame, columns: list, expected: pd.DataFrame):
    sdf = qmpc.send_to(df)
    result = sdf.mean(columns).to_data_frame()
    pd.testing.assert_frame_equal(result, expected)
