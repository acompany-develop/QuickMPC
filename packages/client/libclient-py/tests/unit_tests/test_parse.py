import os
from typing import List

import numpy as np
import pytest

from quickmpc.utils.parse_csv import (parse, parse_csv, parse_csv_to_bitvector,
                                      parse_to_bitvector)

# 元データ
data1: List[List[str]] = [s.split(",") for s in [
    "id,attr1,attr2,attr3,attr4,attr5,attr6",
    "hoge,0,0.77,0.63,0.35,0.39,0.35",
    "huga,0,0.37,0.36,0.43,0.41,0.39",
    "piyo,1,0.34,0.34,0.44,0.50,0.32",
    "moge,1,0.47,0.43,0.34,0.29,0.34",
    "moga,0,0.67,0.41,0.25,0.49,0.25",
]]
data2: List[List[str]] = [s.split(",") for s in [
    "id,attr1,attr2,attr3",
    "hoge,0,1,3",
    "huga,0,2,1",
    "moge,1,0,4",
    "moga,0,1,2",
]]

# 正しくparseされたデータ
d1_schema: List[str] = ['id', 'attr1', 'attr2',
                        'attr3', 'attr4', 'attr5', 'attr6']
d1_secrets: List[List[float]] = [
    [230379555.4797964, 0, 0.77, 0.63, 0.35, 0.39, 0.35],
    [10723675.973257065, 0, 0.37, 0.36, 0.43, 0.41, 0.39],
    [117576607.23670769, 1, 0.34, 0.34, 0.44, 0.5, 0.32],
    [211114761.8482437, 1, 0.47, 0.43, 0.34, 0.29, 0.34],
    [13292676.303739548, 0, 0.67, 0.41, 0.25, 0.49, 0.25]
]

d2_schema: List[str] = ['id#0', 'attr1#0', 'attr1#1',
                        'attr2#0', 'attr2#1', 'attr2#2',
                        'attr3#0', 'attr3#1', 'attr3#2', 'attr3#3']
d2_secrets: List[List[float]] = [
    [230379555.4797964, 1, 0, 1, 0, 0, 1, 0, 0, 0],
    [10723675.973257065, 1, 0, 0, 1, 0, 0, 1, 0, 0],
    [211114761.8482437, 0, 1, 0, 0, 1, 0, 0, 1, 0],
    [13292676.303739548, 1, 0, 1, 0, 0, 0, 0, 0, 1]
]


def test_parse():
    """ 正しくパースできるかTest """
    secrets, schema = parse(data1)
    assert (np.allclose(secrets, d1_secrets))
    assert (schema == d1_schema)


def test_parse_to_bitvector():
    """ 正しくパースできるかTest """
    secrets, schema = parse_to_bitvector(data2, [0])
    assert (np.allclose(secrets, d2_secrets))
    assert (schema == d2_schema)


def test_parse_errorhandring():
    """ 異常値を与えてエラーが出るかTest """
    with pytest.raises(Exception):
        # 行が足りずシェアがない
        parse([["id", "a", "b", "c"]])
    with pytest.raises(Exception):
        # schemaに同じものが含まれる
        parse([["id", "a", "a"],
               ["id1", "1", "2"],
               ["id2", "3", "4"]])
    with pytest.raises(Exception):
        # 正方行列でない
        parse([["id", "a", "b"],
               ["id1", "1", "2"],
               ["id2", "3", "4", "5"]])
        parse([["id", "a", "b"],
               ["id1", "1", "2"],
               ["id2"]])


def test_parse_csv():
    """ csvを正しくパースできるかTest """
    secrets, schema = parse_csv(
        f"{os.path.dirname(__file__)}/test_files/data1.csv")
    assert (np.allclose(secrets, d1_secrets))
    assert (schema == d1_schema)


def test_parse_csv_to_bitvector():
    """ csvを正しくパースできるかTest """
    secrets, schema = parse_csv_to_bitvector(
        f"{os.path.dirname(__file__)}/test_files/data3.csv", [0])
    assert (np.allclose(secrets, d2_secrets))
    assert (schema == d2_schema)


def test_parse_csv_errorhandring():
    """ 異常値を与えてエラーが出るかTest """
    with pytest.raises(Exception):
        # fileが存在しない
        parse_csv("hoge")
    with pytest.raises(Exception):
        # formatがおかしい
        parse_csv(f"{os.path.dirname(__file__)}/test_files/data2.csv")
