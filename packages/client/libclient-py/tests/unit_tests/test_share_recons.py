import math
from decimal import Decimal

import numpy as np
import pytest

from quickmpc.exception import ArgumentError
from quickmpc.share import Share


class TestQMPC:

    @staticmethod
    def isclose(val, true_val):
        ok: bool = True
        if isinstance(val, list):
            for v, t in zip(val, true_val):
                ok &= TestQMPC.isclose(v, t)
        elif isinstance(val, dict):
            for v, t in zip(val.values(), true_val.values()):
                ok &= TestQMPC.isclose(v, t)
        else:
            ok &= math.isclose(val, true_val)
        return ok

    @pytest.mark.parametrize(
        ("shares", "expected"),
        [
            # 1次元配列のシェア
            # int
            ([1, 2, 3], 6),
            # float
            ([2.4, 4.2, 4.1], 10.7),
            # Decimal
            ([Decimal(2.4), Decimal(4.2), Decimal(4.1)], Decimal(10.7)),

            # 2次元配列のシェア
            # int
            ([[1, 4], [2, 5], [3, 6]], [6, 15]),
            # float
            ([[1.1, 4.2], [2.3, 5.4], [3.5, 6.6]], [6.9, 16.2]),
            # Decimal
            ([[Decimal(1.1), Decimal(4.2)], [Decimal(2.3), Decimal(5.4)],
              [Decimal(3.5), Decimal(6.6)]], [Decimal(6.9), Decimal(16.2)]),

            # 3次元配列のシェア
            # int
            ([[[1, 4], [7, 8]],
              [[2, 5], [9, 10]],
              [[3, 6], [11, 12]]],
                [[6, 15], [27, 30]]),
            # float
            ([[[1.1, 4.2], [7.1, 8.2]],
              [[2.3, 5.4], [9.1, 10.2]],
              [[3.5, 6.6], [11.1, 12.2]]],
             [[6.9, 16.2], [27.3, 30.6]]),
            # Decimal
            ([[[Decimal(1.1), Decimal(4.2)], [Decimal(7.1), (8.2)]],
              [[Decimal(2.3), Decimal(5.4)], [Decimal(9.1), (10.2)]],
              [[Decimal(3.5), Decimal(6.6)], [Decimal(11.1), (12.2)]]],
             [[Decimal(6.9), Decimal(16.2)], [Decimal(27.3), (30.6)]]),

        ]
    )
    def test_recons_list(self, shares: list, expected):
        """ 3パーティの復元が正しくできるかTest """
        secrets: list = Share.recons(shares)
        assert TestQMPC.isclose(secrets, expected)

    @pytest.mark.parametrize(
        ("shares", "expected"),
        [
            # 辞書型配列のシェア
            ([{"a": 1}, {"a": 2}, {"a": 3}],
             {"a": 6}),
            ([{"a": 1.1, "b": 2}, {"a": 4.1, "b": 9.1}, {"a": 3.1, "b": -2.1}],
             {"a": 8.3, "b": 9}),
            ([{"a": 1.1, "child": {"b": 2, "c": [1, 3]}},
              {"a": 4.1, "child": {"b": 9.1, "c": [2, 1]}},
              {"a": 3.1, "child": {"b": -2.1, "c": [1, 8]}}],
             {"a": 8.3, "child": {"b": 9, "c": [4, 12]}}),
            # 辞書型配列の配列のシェア
            ([[{"a": 1}], [{"a": 2}], [{"a": 3}]],
             [{"a": 6}]),
            ([[{"a": 1.1}, {"b": 2}],
                [{"a": 3.1}, {"b": 9.1}],
                [{"a": 4.1}, {"b": -2.1}]],
                [{"a": 8.3}, {"b": 9}]),
            ([[{"a": 1.1}, {"child": {"b": 2, "c": [1, 3]}}],
                [{"a": 4.1}, {"child": {"b": 9.1, "c": [2, 1]}}],
                [{"a": 3.1}, {"child": {"b": -2.1, "c": [1, 8]}}]],
                [{"a": 8.3}, {"child": {"b": 9, "c": [4, 12]}}]),
        ]
    )
    def test_recons_dict(self, shares: dict, expected: dict):
        """ 3パーティの辞書型の復元が正しくできるかTest """
        secrets: list = Share.recons(shares)
        assert TestQMPC.isclose(secrets, expected)

    @pytest.mark.parametrize(
        ("shares", "expected"),
        [
            # 2パーティのシェア
            ([[1, 5], [2, 9]], [3, 14]),
            ([[[1, 2], [3, 4]], [[5, 6], [7, 8]]], [[6, 8], [10, 12]]),
            ([[2.4, 9.1], [4.2, 6.7], [4.1, 1.5]], [10.7, 17.3]),
            # 4パーティのシェア
            ([[3000], [2500], [200], [400]], [6100]),
            ([[[3000]], [[2500]], [[200]], [[400]]], [[6100]]),
            # 5パーティのシェア
            ([[3000], [2500], [200], [400], [11235]], [17335]),
            ([[[3000]], [[2500]], [[200]], [[400]], [[11235]]], [[17335]]),
        ]
    )
    def test_recons_multi(self, shares: list, expected: list):
        """ nパーティのシェアの復元が正しくできるかTest """
        secrets: list = Share.recons(shares)
        assert TestQMPC.isclose(secrets, expected)

    @pytest.mark.parametrize(
        ("shares", "expected"),
        [
            ([1e1200 for _ in range(3)], 3e1200),
            ([Decimal(1e1200) for _ in range(3)], Decimal(3e1200)),
            ([-1e1200 for _ in range(3)], -3e1200),
            ([Decimal(-1e1200) for _ in range(3)], Decimal(-3e1200)),
            ([1e-50 for _ in range(3)], 3e-50),
            ([Decimal(1e-50) for _ in range(3)], Decimal(3e-50)),
            ([-1e-50 for _ in range(3)], -3e-50),
            ([Decimal(-1e-50) for _ in range(3)], Decimal(-3e-50)),
        ]
    )
    def test_recons_edge(self, shares: list, expected):
        """ 3パーティの復元が正しくできるかTest """
        secrets = Share.recons(shares)
        assert TestQMPC.isclose(secrets, expected)

    @pytest.mark.parametrize(
        ("shares", "expected"),
        [
            # scalar value is not allowed
            (1, ArgumentError),

            # string value is not allowed
            ("hey", ArgumentError),
            (["hey"], TypeError),  # TODO: quickmpcのArgumentErrorが出るようにしたい
            ([["hey"]], TypeError),
        ]
    )
    def test_recons_errorhandring(self, shares, expected):
        """ 異常値を与えてエラーが出るかTest """
        with pytest.raises(expected):
            Share.recons(shares)

    @pytest.mark.parametrize(
        ("secrets"),
        [
            # 1次元配列の秘密情報
            ([1]), ([3.0, 5.12, 21.05]), ([1, 2, 3, 4, 5, 6, 7, 8, 9, 10]),
            # 2次元配列の秘密情報
            ([[1]]), ([[3000, 500, 2100], [100, 2500, 1100]]), ([[2.1, 3.2]]),
            ([[1, 2, 3, 4, 5],
              [6, 7, 8, 9, 10],
              [11, 12, 13, 14, 15],
              [16, 17, 18, 19, 20],
              [21, 22, 23, 24, 25]])
        ]
    )
    def test_sharize_recons(self, secrets: list):
        """ nパーティのシェア化，復元が正しくできてるかTest"""
        for party_size in range(2, 10):
            shares: list = Share.sharize(secrets, party_size=party_size)
            conved: list = [Share.convert_type(s) for s in shares]
            secrets_2: list = Share.recons(conved)
            secrets_2 = Share.convert_type(secrets_2)
            assert (np.allclose(secrets, secrets_2))
