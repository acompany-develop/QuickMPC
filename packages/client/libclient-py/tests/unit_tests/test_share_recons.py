import math
from decimal import Decimal

import numpy as np
import pytest

from quickmpc.share import Share


class TestQMPC:

    @pytest.mark.parametrize(
        ("shares"),
        [
            # 1次元配列のシェア
            (["1", "2", "3"]),
            (["2.4", "4.2", "4.1"]),
            # 2次元配列のシェア
            ([["1"], ["2"], ["3"]]),
            ([["3000", "500"], ["2500", "1100"], ["100", "200"]]),
            ([["2.4", "9.1"], ["4.2", "6.7"], ["4.1", "1.5"]]),
            # 3次元配列のシェア
            ([[["1"]], [["2"]], [["3"]]]),
            ([[["30.0", "5.22"], ["4.89", "1.001"]], [["25.1", "11.901"],
             ["1200", "400"]], [["100", "200"], ["300", "800"]]])
        ]
    )
    def test_recons_list(self, shares: list):
        """ 3パーティの復元が正しくできるかTest """
        secrets: list = Share.recons(shares)
        shares_float: np.ndarray = \
            np.vectorize(Decimal)(Share.sharize(secrets))
        assert (np.allclose(secrets,
                            np.vectorize(float)(np.sum(shares_float, axis=0))))

    @pytest.mark.parametrize(
        ("shares", "secrets_true"),
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
    def test_recons_dict(self, shares: list, secrets_true: list):
        """ 3パーティの辞書型の復元が正しくできるかTest """
        secrets: list = Share.recons(shares)

        def isclose(val, true_val):
            ok: bool = True
            if isinstance(val, list):
                for v, t in zip(val, true_val):
                    ok &= isclose(v, t)
            elif isinstance(val, dict):
                for v, t in zip(val.values(), true_val.values()):
                    ok &= isclose(v, t)
            else:
                ok &= math.isclose(val, true_val)
            return ok

        assert (isclose(secrets, secrets_true))

    @pytest.mark.parametrize(
        ("shares"),
        [
            # 2パーティのシェア
            ([["1", "5"], ["2", "9"]]),
            ([[["1", "2"], ["3", "4"]], [["5", "6"], ["7", "8"]]]),
            ([["2.4", "9.1"], ["4.2", "6.7"], ["4.1", "1.5"]]),
            # 3次元配列のシェア
            # 4パーティのシェア
            ([["3000"], ["2500"], ["200"], ["400"]]),
            ([[["3000"]], [["2500"]], [["200"]], [["400"]]]),
            # 5パーティのシェア
            ([["3000"], ["2500"], ["200"], ["400"], ["11235"]]),
            ([[["3000"]], [["2500"]], [["200"]], [["400"]], [["11235"]]]),
        ]
    )
    def test_recons_multi(self, shares: list):
        """ nパーティのシェアの復元が正しくできるかTest """
        secrets: list = Share.recons(shares)
        shares_float: np.ndarray = \
            np.vectorize(Decimal)(Share.sharize(secrets))
        assert (np.allclose(secrets,
                            np.vectorize(float)(np.sum(shares_float, axis=0))))

    def test_recons_errorhandring(self):
        """ 異常値を与えてエラーが出るかTest """
        with pytest.raises(Exception):
            Share.recons(1)
        with pytest.raises(Exception):
            Share.recons("hey")

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
            secrets_2: list = Share.recons(shares)
            assert (np.allclose(secrets, secrets_2))

    @pytest.mark.parametrize(
        ("shares"),
        [
            # str
            ([["a", "b"], ["a", "b"]]),
            ([["a", "b", "c"], ["a", "b", "c"], ["a", "b", "c"]])
        ]
    )
    def test_sharize_recons_not_share(self, shares: list):
        """ 数値でない(Shareでない)値を復元せず返却できるかTest """
        secrets: list = Share.recons(shares)
        assert (shares[0] == secrets)
