import pandas as pd
import pytest
from utils import qmpc_new


@pytest.mark.parametrize(
    ("df", "columns1", "columns2",  "expected"),
    [
        # usually case
        (pd.DataFrame([[1, 2, 3], [4, 7, 10], [2, 9, 3]],
                      columns=["s1", "s2", "s3"]),
         [1, 2], [3],
         pd.DataFrame([[0.94491118, 0.24019223]])),


        # large data case
        (pd.DataFrame([[10**9, 10**9+5], [10**9+10, 10**9+10]],
                      columns=["s1", "s2"]),
         [1], [2],
         pd.DataFrame([[1.0]])),

        # small data case
        (pd.DataFrame([[10**-9, 10**-9+5], [10**-9+10, 10**-9+10]],
                      columns=["s1", "s2"]),
         [1], [2],
         pd.DataFrame([[1.0]])),

        # duplicated src case
        (pd.DataFrame([[1, 2, 3], [4, 7, 10], [2, 9, 3]],
                      columns=["s1", "s2", "s3"]),
         [2, 3, 3], [1, 1, 3],
         pd.DataFrame([[0.54470478, 0.94491118, 0.94491118],
                       [0.54470478, 0.94491118, 0.94491118],
                       [0.24019223, 1.0, 1.0]])),
    ]
)
def test_correl(df: pd.DataFrame, columns1: list, columns2: list,
                expected: pd.DataFrame):
    sdf = qmpc_new.send_to(df)
    result = sdf.correl(columns1, columns2).to_data_frame()
    pd.testing.assert_frame_equal(result, expected)
