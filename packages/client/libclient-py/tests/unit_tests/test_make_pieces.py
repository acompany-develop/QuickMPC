from typing import List

import pytest

from quickmpc.exception import ArgmentError
from quickmpc.utils.make_pieces import MakePiece


class TestQMPC:

    MATRIX: List[List[str]] = [[str(i) * i for i in range(1, 6)]] * 5

    @pytest.mark.parametrize(
        ("array, size, expected"),
        [
            (["1", "2", "3", "4"], 4, [["1", "2", "3", "4"]]),
            (["1", "2", "3", "4"], 3, [["1", "2", "3"], ["4"]]),
            (["1", "2", "3", "4"], 2, [["1", "2"], ["3", "4"]]),
        ]
    )
    def test_make_pieces_array(self, array, size, expected):
        actual = MakePiece.make_pieces(array, size)
        assert (actual == expected)

    @pytest.mark.parametrize(
        ("matrix, size, expected"),
        [
            (MATRIX, 75, [MATRIX]),
            (MATRIX, 30, [MATRIX[:2], MATRIX[2:4], MATRIX[4:]]),
            (MATRIX[0], 15, [MATRIX[0]])
        ]
    )
    def test_make_pieces(self, matrix, size, expected):
        actual = MakePiece.make_pieces(matrix, size)
        assert (actual == expected)

    @pytest.mark.parametrize(
        ("string, size, expected"),
        [
            # 空文字列
            ("", 1, []),
            ("", 3, []),
            # 短めの文字列
            ("aaabbbcccddd", 6, ["aaabbb", "cccddd"]),
            ("aaabbbcccddd", 9, ["aaabbbccc", "ddd"]),
            ("aaabbbcccddd", 12, ["aaabbbcccddd"]),
            ("aaabbbcccddd", 15, ["aaabbbcccddd"]),
            # 長めの文字列
            ("a" * 1_000_000, 1, ["a" * 1] * 1_000_000),
            ("a" * 1_000_000, 1_000, ["a" * 1_000] * 1_000),
            ("a" * 1_000_000, 1_000_000, ["a" * 1_000_000] * 1),
            # 特殊な文字
            ("!\"#$%&'()=~|-^@`[]{};+:*,<.>/?", 5,
             ["!\"#$%", "&'()=", "~|-^@", "`[]{}", ";+:*,", "<.>/?"]),
        ]
    )
    def test_make_pieces_string(self, string, size, expected):
        actual = MakePiece.make_pieces(string, size)
        assert (actual == expected)

    @pytest.mark.parametrize(
        ("args, error"),
        [
            ([MATRIX, 0], RuntimeError),          # サイズが小さい場合
            ([MATRIX, 1_000_001], RuntimeError),  # サイズが大きい場合
            ([0, 0], ArgmentError),               # 引数タイプが無効な場合: 0 次元
            ([[MATRIX], 0], ArgmentError),        # 引数タイプが無効な場合: 3 次元
        ]
    )
    def test_make_pieces_errorhandring(self, args, error):
        """ 異常値を与えてエラーが出るかTest """
        with pytest.raises(error):
            MakePiece.make_pieces(*args)

    @pytest.mark.parametrize(
        ("args, error"),
        [
            (["string", 0], RuntimeError),          # サイズが小さい場合
            (["string", 1_000_001], RuntimeError),  # サイズが大きい場合
        ]
    )
    def test_make_pieces_str_errorhandring(self, args, error):
        """ 異常値を与えてエラーが出るかTest """
        with pytest.raises(error):
            MakePiece.make_pieces(*args)
