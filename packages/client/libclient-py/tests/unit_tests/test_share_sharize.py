import math
from decimal import Decimal
from typing import List

import numpy as np
import pytest

from quickmpc.exception import ArgumentError
from quickmpc.share import Share


def sharize_params(secrets=[1], party_size=3):
    return (secrets, party_size)


class TestQMPC:

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
    def test_sharize(self, secrets: list):
        """ nパーティのシェア化が正しくできているかのTest """
        for party_size in range(2, 10):
            shares: np.ndarray = np.vectorize(Decimal)(
                Share.sharize(secrets, party_size=party_size))
            assert (len(shares) == party_size)
            assert (np.allclose(secrets,
                                np.vectorize(float)(np.sum(shares, axis=0))))

    @pytest.mark.parametrize(
        ("secrets"),
        [(1), (0.4), (-100), (-50.14)]
    )
    def test_sharize_scalar(self, secrets: list):
        """ nパーティのスカラ値のシェア化が正しくできているかのTest """
        for party_size in range(2, 10):
            shares: np.ndarray = np.vectorize(Decimal)(
                Share.sharize(secrets, party_size=party_size))
            assert (len(shares) == party_size)
            assert (float(np.sum(shares)) == pytest.approx(secrets))

    @pytest.mark.parametrize(
        ("secrets"),
        [
            ([{"a": 3, "b": 2}]),
            ([{"a": 3, "b": {"b1": 1, "b2": 10}}]),
            ([{"a": 3, "b": {"b1": 1, "b2": {"b21": {"b211": 100}}}},
              {"c": 3, "d": {"d1": 1, "d2": {"d21": {"d211": 100}}}}]),
        ]
    )
    def test_sharize_json(self, secrets: List[dict]):
        """ nパーティのjsonのシェア化が正しくできているかのTest """
        def dfs(val, key=""):
            dc: dict = dict()
            if isinstance(val, list):
                for v in val:
                    dc = {**dc, **dfs(v, key)}
                return dc
            if isinstance(val, dict):
                for k, v in val.items():
                    dc = {**dc, **dfs(v, key + k)}
                return dc
            return {**dc, **{key: val}}
        true_map: dict = dfs(secrets)

        for party_size in range(2, 10):
            share_json = Share.sharize(secrets, party_size=party_size)
            share_map: dict = dict()
            for share in share_json:
                for key, val in dfs(share).items():
                    if key not in share_map:
                        share_map[key] = Decimal(0)
                    share_map[key] += Decimal(val)
            for key, _ in dfs(share).items():
                assert (np.isclose(float(share_map[key]), true_map[key]))

    @pytest.mark.parametrize(
        ("secrets"),
        [
            # 1次元配列の秘密情報
            ([1e999]),
            # 2次元配列の秘密情報
            ([[1e999]])
        ]
    )
    def test_sharize_convert_to_inf(self, secrets: list):
        """ Infinityがinfに変換されているかのTest """
        for party_size in range(2, 10):
            shares = Share.sharize(secrets, party_size=party_size)

            def is_inf(val):
                ok: bool = True
                if isinstance(val, list):
                    for v in val:
                        ok &= is_inf(v)
                else:
                    ok &= (val == 'inf')
                return ok

            assert (is_inf(shares[0]))

    @pytest.mark.parametrize(
        ("secrets"),
        [
            # zero
            (0),
            # int_max
            (1e1200),
            # int_min
            (-1e1200),
            # float_min_plus
            (1e-50),
            # float_min_minus
            (-1e-50),
        ]
    )
    def test_sharize_edge(self, secrets: float):
        """ nパーティのシェア化が正しくできているかのTest """
        for party_size in range(2, 10):
            shares: np.ndarray = np.vectorize(Decimal)(
                Share.sharize(secrets, party_size=party_size))
            assert (len(shares) == party_size)
            assert math.isclose(secrets, sum(shares), abs_tol=1e-5)

    @pytest.mark.parametrize(
        ("args", "expected_exception"),
        [
            # string
            (sharize_params(secrets="str"), ArgumentError),
            # array string
            (sharize_params(secrets="[str]"), ArgumentError),
            # 列が異なる
            (sharize_params(secrets=[[1, 2], [1]]), ValueError),
            # 3-dim
            (sharize_params(secrets=[[[1]]]), ArgumentError),
            # empty
            (sharize_params(secrets=[[], []]), ValueError),
            # party_sizeが1
            (sharize_params(party_size=1), ArgumentError),
        ]
    )
    def test_sharize_errorhandring(self, args, expected_exception):
        """ 異常値を与えてエラーが出るかTest """
        with pytest.raises(expected_exception):
            Share.sharize(*args)
