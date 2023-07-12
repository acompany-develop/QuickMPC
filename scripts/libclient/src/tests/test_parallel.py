from concurrent.futures import ThreadPoolExecutor

import pandas as pd
import pytest
from utils import data_frame, qmpc

df = data_frame([[1, 2], [3, 4]], columns=["s1", "s2"])


@pytest.fixture(scope="module")
def correct_sdf():
    return qmpc.send_to(df)


@pytest.fixture(scope="module")
def correct_result(correct_sdf):
    return correct_sdf.sum([1, 2])


@pytest.mark.parametrize(
    ("parallel_num"), [
        (1), (2), (3), (4), (5), (10)
    ]
)
def test_send_share(parallel_num: int):
    futures = []
    with ThreadPoolExecutor() as executor:
        for _ in range(parallel_num):
            futures.append(executor.submit(qmpc.send_to, df))
    for future in futures:
        # raiseされないかチェック
        future.result()


@pytest.mark.parametrize(
    ("parallel_num"), [
        (1), (2), (3), (4), (5), (10)
    ]
)
def test_execute(parallel_num: int, correct_sdf):
    futures = []
    with ThreadPoolExecutor() as executor:
        for _ in range(parallel_num):
            futures.append(executor.submit(correct_sdf.sum, [1, 2]))
    expected = pd.DataFrame([4.0, 6.0])
    for future in futures:
        result = future.result().to_data_frame()
        pd.testing.assert_frame_equal(result, expected)


@pytest.mark.parametrize(
    ("parallel_num"), [
        (1), (2), (3), (4), (5), (10)
    ]
)
def test_get_computation_result(parallel_num: int, correct_result):
    futures = []
    with ThreadPoolExecutor() as executor:
        for _ in range(parallel_num):
            futures.append(executor.submit(correct_result.to_data_frame))
    expected = pd.DataFrame([4.0, 6.0])
    for future in futures:
        result = future.result()
        pd.testing.assert_frame_equal(result, expected)
