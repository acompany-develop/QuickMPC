import os
import shutil

import pandas as pd
import pytest
from utils import data_frame, qmpc


@pytest.fixture(scope="module")
def sdf():
    return qmpc.send_to(data_frame([[1, 2], [3, 4]],
                                   columns=["s1", "s2"]))


@pytest.mark.parametrize(
    ("method", "args", "expected"),
    [
        # 1次元結果の取得
        ("sum", ([1, 2],),
         pd.DataFrame([4.0, 6.0])),
        # # 2次元結果の取得
        # ("correl", ([1, 2], [1, 2]),
        #  pd.DataFrame([[1.0, 1.0], [1.0, 1.0]])),
        # テーブルデータの取得
        ("join", ([],),
         pd.DataFrame([[2.0], [4.0]], columns=["s2"])),
    ]
)
def test_to_data_frame(method: str, args: dict, expected: pd.DataFrame,
                       sdf):
    result = getattr(sdf, method)(*args).to_data_frame()
    pd.testing.assert_frame_equal(result, expected)


@pytest.mark.parametrize(
    ("method", "args", "expected"),
    [
        # 1次元結果の取得
        ("sum", ([1, 2],),
         pd.DataFrame([4.0, 6.0])),
        # 2次元結果の取得
        ("correl", ([1, 2], [1, 2]),
         pd.DataFrame([[1.0, 1.0], [1.0, 1.0]])),
        # テーブルデータの取得
        ("join", ([],),
         pd.DataFrame([[2.0], [4.0]], columns=["s2"])),
    ]
)
def test_load_job_uuid(method: str, args: dict, expected: pd.DataFrame,
                       sdf):
    sdf = getattr(sdf, method)(*args)
    result = qmpc.load_from(sdf.get_id()).to_data_frame()
    pd.testing.assert_frame_equal(result, expected)


@pytest.mark.parametrize(
    ("method", "args", "expected"),
    [
        # 1次元結果の取得
        ("sum", ([1, 2],),
         pd.DataFrame([4.0, 6.0])),
        # # 2次元結果の取得
        # ("correl", ([1, 2], [1, 2]),
        #  pd.DataFrame([[1.0, 1.0], [1.0, 1.0]])),
        # テーブルデータの取得
        ("join", ([],),
         pd.DataFrame([[2.0], [4.0]], columns=["s2"])),
    ]
)
def test_restore(method: str, args: dict, expected: pd.DataFrame,
                 sdf):
    path = "./result"
    if os.path.isdir(path):
        shutil.rmtree(path)
    os.mkdir(path)

    sdf_result = getattr(sdf, method)(*args)
    sdf_result.to_csv(path)
    result = qmpc.restore(sdf_result.get_id(), path, 3)
    pd.testing.assert_frame_equal(result, expected)

    shutil.rmtree(path)
