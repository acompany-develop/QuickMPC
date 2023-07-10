import pandas as pd
import pytest
from utils import qmpc


@pytest.mark.parametrize(
    ("df", "columns", "expected"),
    [
        # usually case
        (pd.DataFrame([[1, 2, 3], [4, 7, 10]], columns=["s1", "s2", "s3"]),
         [1, 2, 3],
         pd.DataFrame([2.25, 6.25, 12.25])),

        # small table size case
        (pd.DataFrame([[1]], columns=["s1"]),
         [1],
         pd.DataFrame([0.0])),

        # large data case
        (pd.DataFrame([[10**9], [10**9+10]], columns=["s1"]),
         [1],
         pd.DataFrame([25.0])),

        # small data case
        (pd.DataFrame([[10**-9], [10**-9+10]], columns=["s1"]),
         [1],
         pd.DataFrame([25.0])),

        # duplicated src case
        (pd.DataFrame([[1, 2, 3], [4, 7, 10]], columns=["s1", "s2", "s3"]),
         [1, 2, 2, 3, 3, 3],
         pd.DataFrame([2.25, 6.25, 6.25, 12.25, 12.25, 12.25])),

        # variance is 0 case
        (pd.DataFrame([[1], [1], [1]], columns=["s1"]),
         [1],
         pd.DataFrame([0.0])),
    ]
)
def test_variance(df: pd.DataFrame, columns: list, expected: pd.DataFrame):
    sdf = qmpc.send_to(df)
    result = sdf.variance(columns).to_data_frame()
    pd.testing.assert_frame_equal(result, expected, atol=1e-5)
