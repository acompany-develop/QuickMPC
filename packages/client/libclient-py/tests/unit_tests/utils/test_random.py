from decimal import Decimal

import pytest

from quickmpc.utils.random import ChaCha20, RandomInterface


class TestCsprng:
    rnd: RandomInterface = ChaCha20()

    @pytest.mark.parametrize(
        ("lower", "upper"),
        [
            (0, 10),  # 正のみ
            (-5, 5),  # 正負
            (-10, 0),  # 負のみ
            (0, 1),   # 範囲が1
            (-(1 << 128), -(1 << 128) + 1),  # 128bit最小値
            ((1 << 128) - 1, (1 << 128) + 1)  # 128bit最大値
        ]
    )
    def test_csprng_int_interval(self, lower: int, upper: int):
        """ 半開区間内で生成されるかのtest """
        exist_lower: bool = False
        exist_upper_1: bool = False
        for _ in range(1000):
            x: int = self.rnd.get(lower, upper)
            assert (lower <= x and x < upper)
            assert (isinstance(x, int))
            exist_lower |= (x == lower)
            exist_upper_1 |= (x == upper - 1)
        for x in self.rnd.get_list(lower, upper, 1000):
            assert (lower <= x and x < upper)
            assert (isinstance(x, int))
            exist_lower |= (x == lower)
            exist_upper_1 |= (x == upper - 1)
        assert (exist_lower)
        assert (exist_upper_1)

    @pytest.mark.parametrize(
        ("lower", "upper"),
        [
            (Decimal(0.0), Decimal(10.0)),  # 正のみ
            (Decimal(-5.0), Decimal(5.0)),  # 正負
            (Decimal(-10.0), Decimal(0.0)),  # 負のみ
            (Decimal(0.0), Decimal(1.0)),   # 範囲が1
            (Decimal(-(1 << 64)), Decimal(-(1 << 64) + 1)),
            (Decimal((1 << 64) - 1), Decimal((1 << 64) + 1))
        ]
    )
    def test_csprng_decimal_interval(self, lower: Decimal, upper: Decimal):
        """ 半開区間内で生成されるかのtest """
        for _ in range(1000):
            x: int = self.rnd.get(lower, upper)
            assert (lower <= x and x < upper)
            assert (isinstance(x, Decimal))
        for x in self.rnd.get_list(lower, upper, 1000):
            assert (lower <= x and x < upper)
            assert (isinstance(x, Decimal))

    @pytest.mark.parametrize(
        ("lower", "upper"),
        [
            (0, 0),
            (10, 0),
            (0, 1.2),
            (0.0, 1),
            ("hoge", "huga"),
        ]
    )
    def test_csprng_errorhandring(self, lower, upper):
        """ 異常値を与えてエラーが出るかtest """
        with pytest.raises(Exception):
            self.rnd.get(lower, upper)
