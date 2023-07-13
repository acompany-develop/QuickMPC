import pandas as pd
import pytest
from utils import data_frame, qmpc


@pytest.mark.parametrize(
    ("df1", "df2", "expected"),
    [
        # usually case
        (data_frame([[10, 1, 2], [11, 1, 2]], columns=["id", "s1", "s2"]),
         data_frame([[10, 3, 4], [12, 3, 4]], columns=["id", "s3", "s4"]),
         pd.DataFrame([[1.0, 2.0, 3.0, 4.0]],
                      columns=["s1", "s2", "s3", "s4"])),

        # all match case
        (data_frame([[10, 1, 2], [11, 1, 2]], columns=["id", "s1", "s2"]),
         data_frame([[10, 3, 4], [11, 3, 4]], columns=["id", "s3", "s4"]),
         pd.DataFrame([[1.0, 2.0, 3.0, 4.0], [1.0, 2.0, 3.0, 4.0]],
                      columns=["s1", "s2", "s3", "s4"])),

        # not match case
        (data_frame([[10, 1, 2], [11, 1, 2]], columns=["id", "s1", "s2"]),
         data_frame([[20, 3, 4], [21, 3, 4]], columns=["id", "s3", "s4"]),
         pd.DataFrame([], columns=["s1", "s2", "s3", "s4"])),

        # empty table case
        (data_frame([[10]], columns=["id"]),
         data_frame([[10]], columns=["id"]),
         pd.DataFrame([], columns=[])),

        # large table case
        (data_frame([[id, 1] for id in range(1000)], columns=["id", "s1"]),
         data_frame([[id, 2] for id in range(1000)], columns=["id", "s2"]),
         pd.DataFrame([[1.0, 2.0] for id in range(1000)], columns=["s1", "s2"])),
    ]
)
def test_hjoin(df1: pd.DataFrame, df2: pd.DataFrame, expected: pd.DataFrame):
    sdf1 = qmpc.send_to(df1)
    sdf2 = qmpc.send_to(df2)
    result = sdf1.join(sdf2, debug_mode=True).to_data_frame()
    pd.testing.assert_frame_equal(result, expected)


@pytest.mark.parametrize(
    ("df1", "df2", "expected"),
    [
        # usually case
        (data_frame([[10, 1, 2], [11, 1, 2]], columns=["id", "s1", "s2"]),
         data_frame([[10, 3, 4], [12, 3, 4]], columns=["id", "s3", "s4"]),
         pd.DataFrame([[1.0, 2.0, 3.0, 4.0]],
                      columns=["s1", "s2", "s3", "s4"])),

        # all match case
        (data_frame([[10, 1, 2], [11, 1, 2]], columns=["id", "s1", "s2"]),
         data_frame([[10, 3, 4], [11, 3, 4]], columns=["id", "s3", "s4"]),
         pd.DataFrame([[1.0, 2.0, 3.0, 4.0], [1.0, 2.0, 3.0, 4.0]],
                      columns=["s1", "s2", "s3", "s4"])),

        # not match case
        (data_frame([[10, 1, 2], [11, 1, 2]], columns=["id", "s1", "s2"]),
         data_frame([[20, 3, 4], [21, 3, 4]], columns=["id", "s3", "s4"]),
         pd.DataFrame([], columns=["s1", "s2", "s3", "s4"])),

        # empty table case
        (data_frame([[10]], columns=["id"]),
         data_frame([[10]], columns=["id"]),
         pd.DataFrame([], columns=[])),

        # large table case
        (data_frame([[id, 1] for id in range(1000)], columns=["id", "s1"]),
         data_frame([[id, 2] for id in range(1000)], columns=["id", "s2"]),
         pd.DataFrame([[1.0, 2.0] for id in range(1000)],
                      columns=["s1", "s2"])),
    ]
)
def test_hjoin_share(df1: pd.DataFrame, df2: pd.DataFrame,
                     expected: pd.DataFrame):
    sdf1 = qmpc.send_to(df1)
    sdf2 = qmpc.send_to(df2)
    result = sdf1.join(sdf2).to_data_frame()
    pd.testing.assert_frame_equal(result, expected)
