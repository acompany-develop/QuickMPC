import math
import os
from typing import List

import numpy as np
import pytest

from quickmpc import Schema, ShareValueTypeEnum
from quickmpc.utils.parse_csv import (parse, parse_csv, parse_csv_to_bitvector,
                                      parse_to_bitvector)

# 元データ
normal_data: List[List[str]] = [s.split(",") for s in [
    "id,attr1,attr2,attr3,attr4,attr5,attr6",
    "hoge,0,0.77,0.63,0.35,0.39,0.35",
    "huga,0,0.37,0.36,0.43,0.41,0.39",
    "piyo,1,0.34,0.34,0.44,0.50,0.32",
    "moge,1,0.47,0.43,0.34,0.29,0.34",
    "moga,0,0.67,0.41,0.25,0.49,0.25",
]]
bitvector_data: List[List[str]] = [s.split(",") for s in [
    "id,attr1,attr2,attr3",
    "hoge,0,1,3",
    "huga,0,2,1",
    "moge,1,0,4",
    "moga,0,1,2",
]]
data3: List[List[str]] = [s.split(",") for s in [
    "id,id:id",
    "hoge,hoge",
    "huga,huga",
    "moge,moge",
    "moga,moga",
]]

# 正しくparseされたデータ
d1_schema_str: List[str] = ['id', 'attr1', 'attr2',
                            'attr3', 'attr4', 'attr5', 'attr6']
d1_schema: List[Schema] = [
    Schema(
        name=name,
        type=ShareValueTypeEnum.SHARE_VALUE_TYPE_FIXED_POINT)
    for name in d1_schema_str]
d1_secrets: List[List[float]] = [
    [230379555.4797964, 0, 0.77, 0.63, 0.35, 0.39, 0.35],
    [10723675.973257065, 0, 0.37, 0.36, 0.43, 0.41, 0.39],
    [117576607.23670769, 1, 0.34, 0.34, 0.44, 0.5, 0.32],
    [211114761.8482437, 1, 0.47, 0.43, 0.34, 0.29, 0.34],
    [13292676.303739548, 0, 0.67, 0.41, 0.25, 0.49, 0.25]
]

d2_schema_str: List[str] = ['id#0', 'attr1#0', 'attr1#1',
                            'attr2#0', 'attr2#1', 'attr2#2',
                            'attr3#0', 'attr3#1', 'attr3#2', 'attr3#3']
d2_schema: List[Schema] = [
    Schema(
        name=name,
        type=ShareValueTypeEnum.SHARE_VALUE_TYPE_FIXED_POINT)
    for name in d2_schema_str]
d2_secrets: List[List[float]] = [
    [230379555.4797964, 1, 0, 1, 0, 0, 1, 0, 0, 0],
    [10723675.973257065, 1, 0, 0, 1, 0, 0, 1, 0, 0],
    [211114761.8482437, 0, 1, 0, 0, 1, 0, 0, 1, 0],
    [13292676.303739548, 1, 0, 1, 0, 0, 0, 0, 0, 1]
]

d3_schema: List[Schema] = [
    Schema(
        name='id',
        type=ShareValueTypeEnum.SHARE_VALUE_TYPE_UTF_8_INTEGER_REPRESENTATION),
    Schema(
        name='id:id',
        type=ShareValueTypeEnum.SHARE_VALUE_TYPE_FIXED_POINT),
]
d3_secrets: List[List[float]] = [
    [1752131429, 230379555.4797964],
    [1752524641, 10723675.973257065],
    [1836017509, 211114761.8482437],
    [1836017505, 13292676.303739548]
]


def test_parse():
    """ 正しくパースできるかTest """
    secrets, schema = parse(normal_data, matching_column=1)
    assert (np.allclose(secrets, d1_secrets))
    assert (schema == d1_schema)


def test_parse_to_bitvector():
    """ 正しくパースできるかTest """
    secrets, schema = parse_to_bitvector(
        bitvector_data, [0], matching_column=1)
    assert (np.allclose(secrets, d2_secrets))
    assert (schema == d2_schema)


def test_parse_str():
    secrets, schema = parse(data3)
    assert (np.allclose(secrets, d3_secrets))
    assert (schema == d3_schema)


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


@pytest.mark.parametrize(
    ("csv_file", "expected_secrets", "expected_schema"),
    [
        # 動作確認
        ("normal.csv", d1_secrets, d1_schema),

        # エッジケース
        ("edge_data.csv",
         [[0.0, 0.0, 10000000000.0, -10000000000.0, 1e-11, -1e-11,
           64397529887256390100288588569511327578050402500065009644001961671253470817989385717477849086243018277720181988562892625970110012333681471039837725745607187030800060309020333162876456608685909824737553037566942120261537940902468758211163720801758556838272012517628989294549513302739055543057036264566886569765390160600458927845218460215249518930528396775656453895555032670341047117802365457053218125987373321700952726750445826896702515318007915452833751068319637097352589402147037401510643116481468848596766486584746707218145923849790876466338]],
         [
             Schema(name='id',
                    type=ShareValueTypeEnum.SHARE_VALUE_TYPE_FIXED_POINT),
             Schema(name='zero',
                    type=ShareValueTypeEnum.SHARE_VALUE_TYPE_FIXED_POINT),
             Schema(name='int_max',
                    type=ShareValueTypeEnum.SHARE_VALUE_TYPE_FIXED_POINT),
             Schema(name='int_min',
                    type=ShareValueTypeEnum.SHARE_VALUE_TYPE_FIXED_POINT),
             Schema(name='float_min_plus',
                    type=ShareValueTypeEnum.SHARE_VALUE_TYPE_FIXED_POINT),
             Schema(name='float_min_minus',
                    type=ShareValueTypeEnum.SHARE_VALUE_TYPE_FIXED_POINT),
             Schema(name='string_max',
                    type=ShareValueTypeEnum.SHARE_VALUE_TYPE_UTF_8_INTEGER_REPRESENTATION),
         ]),

        # 文字列
        ("string_data.csv",
         [[0.0,
           64376492020959182960102910068921920137578270955447009942293249972966067239097553347435029707189797970623272423014052591589722,
           1882191147527907269938743937856638795611151343084998741952969572035357165535685798000133453408036689601601033880933580708456195274283510839478163199706819775268653239200782547247593005475186829113621277019627540944278401988123354850270368777761874656453163621091877363485079081268316782656812385985012357731022634987223268664390185439133540846330425301682911916053212936940769061936237574578117604085883522538530165827831224581273797471035903077033265715119096662560536413065467878476364722979919368009523441872290151221507683287637256104630532783466581111733919248577632948066922190855939936375343591571891468414377755286,
           149125426588557526094103651033825493248966348867408767647059014126948793451002282981754829921684279298438903311001642333143441433438704924300538949095329421911471820491549337260451845790398930276811325635593238559403894042076876848446339967431538398547301138620521332048182374493629643773074940611526758795121183888951643057035805040602930223780366146649584163693840624538042686262318917131715286878370613211773451673474674673254532557304972825179677978128130795383492804612570339102824650467797575298125460982440036007001126374300976444985639952975760824439920591075847525994360947110332554548784783796109833618687851418330161690984711082784722224058,
           2811967216787272001955214519044582659629689727294703346740607363576652787689147056884078078276981761405396389018008701107809927286173590891114641,
           1234567890]],
         [
             Schema(name='id',
                    type=ShareValueTypeEnum.SHARE_VALUE_TYPE_FIXED_POINT),
             Schema(name='alphabet',
                    type=ShareValueTypeEnum.SHARE_VALUE_TYPE_UTF_8_INTEGER_REPRESENTATION),
             Schema(name='hiragana',
                    type=ShareValueTypeEnum.SHARE_VALUE_TYPE_UTF_8_INTEGER_REPRESENTATION),
             Schema(name='katakana',
                    type=ShareValueTypeEnum.SHARE_VALUE_TYPE_UTF_8_INTEGER_REPRESENTATION),
             Schema(name='chinese_characters',
                    type=ShareValueTypeEnum.SHARE_VALUE_TYPE_UTF_8_INTEGER_REPRESENTATION),
             # TODO: 文字列として解釈してほしい
             Schema(name='large_number',
                    type=ShareValueTypeEnum.SHARE_VALUE_TYPE_FIXED_POINT),
         ]),
    ]
)
def test_parse_csv(csv_file, expected_secrets, expected_schema):
    """ csvを正しくパースできるかTest """
    secrets, schema = parse_csv(
        f"{os.path.dirname(__file__)}/test_files/{csv_file}",
        matching_column=1)
    for row, row_expected in zip(secrets, expected_secrets):
        for x, y in zip(row, row_expected):
            if type(x) == int:
                x == y
            else:
                math.isclose(x, y)
    assert (schema == expected_schema)


def test_parse_csv_to_bitvector():
    """ csvを正しくパースできるかTest """
    secrets, schema = parse_csv_to_bitvector(
        f"{os.path.dirname(__file__)}/test_files/bitvector.csv",
        [0], matching_column=1)
    assert (np.allclose(secrets, d2_secrets))
    assert (schema == d2_schema)


@pytest.mark.parametrize(
    ("csv_file", "expected_exception"),
    [
        # ファイルが存在しない
        ("hoge", Exception),

        # 列数が異なる
        ("diff_col.csv", Exception),

        # テーブルが空
        ("empty.csv", Exception),

        # 空のデータが存在する
        ("none.csv", Exception),

        # csv形式じゃない
        ("not_csv.csv", Exception),
    ]
)
def test_parse_csv_errorhandring(csv_file, expected_exception):
    """ 異常値を与えてエラーが出るかTest """
    with pytest.raises(expected_exception):
        parse_csv(f"{os.path.dirname(__file__)}/test_files/{csv_file}")
