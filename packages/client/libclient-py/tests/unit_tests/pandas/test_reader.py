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
        # IDの順序がそのまま
        ([["id", "c"], ["a", 1], ["b", 2]], "id",
         pd.DataFrame([[32772040.0, 1, 0], [86407020.0, 2, 1]],
                      columns=["id", "c", "__qmpc_sort_index__"])),
        # IDの順序が逆
        ([["id", "c"], ["b", 2], ["a", 1]], "id",
         pd.DataFrame([[86407020.0, 2, 1], [32772040.0, 1, 0]],
                      columns=["id", "c", "__qmpc_sort_index__"])),
        # 1列目以外をID列に指定した場合
        ([["id", "c"], ["a", 1], ["b", 2]], "c",
         pd.DataFrame([["a", 81786090.20335388, 1], ["b", 67839041.07183933, 0]],
                      columns=["id", "c", "__qmpc_sort_index__"])),
        # ID列が巨大な整数
        ([["id"], [1000000000000000000000000]], "id",
         pd.DataFrame([[250298887.90448284, 0]], columns=["id", "__qmpc_sort_index__"])),
        ([["id"], [-1000000000000000000000000]], "id",
         pd.DataFrame([[146263071.2934265, 0]], columns=["id", "__qmpc_sort_index__"])),
        # ID列が巨大な実数
        ([["id"], [1000000000000000000000000.1111111111111111]], "id",
         pd.DataFrame([[108235191.58269978, 0]], columns=["id", "__qmpc_sort_index__"])),
        ([["id"], [-1000000000000000000000000.1111111111111111]], "id",
         pd.DataFrame([[264110776.18418598, 0]], columns=["id", "__qmpc_sort_index__"])),
    ]
)
def test_read_csv(data, index_col, expected,
                  run_server1, run_server2, run_server3):
    df = qpd.read_csv(to_string_io(data), index_col=index_col)
    pd.testing.assert_frame_equal(df, expected)
