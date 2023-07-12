import io
from typing import List

import pandas as pd
import pytest

import quickmpc.pandas as qpd


def to_string_io(data: List[List]) -> io.StringIO:
    text_data = "\n".join([",".join(map(str, row)) for row in data])
    return io.StringIO(text_data)


@pytest.mark.parametrize(
    ("data", "index_col", "expected"), [
        # 通常パターン
        ([["id", "c"], ["a", 1], ["b", 2]],
         "id",
         pd.DataFrame([[32772040.0, 1], [86407020.0, 2]],
                      columns=["id", "c"],
                      index=[0, 1])),
        # 座圧されたID列がindexにあるかどうか
        ([["id", "c"], ["b", 2], ["a", 1]],
         "id",
         pd.DataFrame([[86407020.0, 2], [32772040.0, 1]],
                      columns=["id", "c"],
                      index=[1, 0])),
        # 1列目以外をID列に指定した場合
        ([["id", "c"], ["a", 1], ["b", 2]],
         "c",
         pd.DataFrame([["a", 1.0], ["b", 2.0]],
                      columns=["id", "c"],
                      index=[0, 1])),
    ]
)
def test_read_csv(data, index_col, expected,
                  run_server1, run_server2, run_server3):
    df = qpd.read_csv(to_string_io(data), index_col=index_col)
    pd.testing.assert_frame_equal(df, expected)
