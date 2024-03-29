
import pandas as pd
import pytest
from utils import data_frame, qmpc


@pytest.mark.parametrize(
    ("df"),
    [
        (data_frame([[1, 2], [3, 4]], columns=["s1", "s2"])),
        (data_frame([[1]], columns=["s1"])),
    ]
)
def test_send_share_from_csv_data(df: pd.DataFrame):
    # throwされないかtest
    qmpc.send_to(df)


@pytest.mark.parametrize(
    ("df"),
    [
        (data_frame([[1, 2], [3, 4]], columns=["s1", "s2"])),
        (data_frame([[1]], columns=["s1"])),
    ]
)
def test_load_data_id(df: pd.DataFrame):
    # throwされないかtest
    sdf = qmpc.send_to(df)
    qmpc.load_from(sdf.get_id())
