
import pandas as pd
import pytest
from utils import qmpc


@pytest.mark.parametrize(
    ("df"),
    [
        (pd.DataFrame([[1, 2], [3, 4]], columns=["s1", "s2"])),
        (pd.DataFrame([[1]], columns=["s1"])),
        (pd.DataFrame([["a", 1], ["b", 2]], columns=["id", "s1"])),
        (pd.DataFrame([["a", "x"], ["b", "y"]], columns=["id", "xy"])),
    ]
)
def test_send_share_from_csv_data(df: pd.DataFrame):
    # throwされないかtest
    qmpc.send_to(df)
