
import pandas as pd
import pytest
from utils import data_frame, qmpc


@pytest.mark.parametrize(
    ("df"),
    [
        (data_frame([[1, 2], [3, 4]], columns=["s1", "s2"])),
        (data_frame([[1]], columns=["s1"])),
        (data_frame([["a", 1], ["b", 2]], columns=["id", "s1"])),
        (data_frame([["a", "x"], ["b", "y"]], columns=["id", "xy"])),
    ]
)
def test_send_share_from_csv_data(df: pd.DataFrame):
    # throwされないかtest
    qmpc.send_to(df)
